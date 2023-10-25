/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "Pir.h"
#include "Photoresistor.h"
#include "Led.h"

//TODO change with (timeClient.getFormattedTime() == "08:00:00" || timeClient.getFormattedTime() == "19:00:00") ? true : false
#define TIME_ROLLED timeClient.getSeconds() == 0 ? true : false
#define LIGHT_THRESHOLD 700

// WiFi credentials
const char* ssid = "OPPOLUCA";
const char* password = "bigo0615";

// MQTT broker settings
const char* mqttServer = "78fbd30dbdf04400a7aefb4b5ee16a5d.s1.eu.hivemq.cloud";
const int mqttPort = 8883;
const char* mqttUsername = "ESP32";
const char* mqttPassword = "Assignament2023";
const char* mqttClientId = "ESP32Client";

// Sensor pins
const int pirPin = 15;  // PIR sensor pin
const int lightSensorPin = 18;  // Photoresistor light sensor pin
const int ledPin = LED_BUILTIN; // LED pin 

// PIR sensor output
int PIRSensorOutput = 0;
SemaphoreHandle_t auto_manual_mutex;

// FreeRTOS task handles
TaskHandle_t rollerTaskHandle;
TaskHandle_t lightTaskHandle;

// FreeRTOS task function prototypes
void rollerTask(void* pvParameters);
void lightTask(void* pvParameters);

// MQTT callback function prototype
void mqttCallback(char* topic, byte* payload, unsigned int length);

// Reconnect functions prototypes
void reconnectMqtt();
void reconnectWiFi();

// WiFi and MQTT clients
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// NTP client
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org");

// Sensors
Pir* pir;
Photoresistor* lightSensor;
Led* led;

bool isTimePassed = false;
bool isAutoEnable = true;

int lastLightState = -1;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  auto_manual_mutex =  xSemaphoreCreateMutex();

  // Initialize PIR
  pir = new Pir(pirPin);
  lightSensor = new Photoresistor(lightSensorPin);
  led = new Led(ledPin);
  led->switchOff();

  // Setup Wi-Fi
  WiFi.begin(ssid, password);

  // Setup MQTT broker
  wifiClient.setInsecure();
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);

  // Setup NTP client
  timeClient.begin();
  timeClient.setTimeOffset(7200);

  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(rollerTask, "Roller Task", 4096, NULL, 1, &rollerTaskHandle, 1);
  xTaskCreatePinnedToCore(lightTask, "Light Task", 4096, NULL, 1, &lightTaskHandle, 1);

  // Turn on LED
  led->switchOn();

  Serial.println("Setup completed!");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }
  
  if (!mqttClient.connected()) {
    reconnectMqtt();
  }

  mqttClient.loop();
  
  timeClient.update();

  if (TIME_ROLLED) {
    isTimePassed = true;
  }

  xSemaphoreTake(auto_manual_mutex, portMAX_DELAY);
  if (xTaskGetHandle("Roller Task") == NULL && isTimePassed && isAutoEnable) {
    xTaskCreatePinnedToCore(rollerTask, "Roller Task", 4096, NULL, 1, &rollerTaskHandle, 1);
    isTimePassed = false;
  }
  xSemaphoreGive(auto_manual_mutex);

  Serial.println("Pir: " + String(pir->detectedMotion()) + ", LightSensor" + String(lightSensor->getIntensity()));

  delay(1000);
}

void rollerTask(void* pvParameters) {
  (void)pvParameters;

  Serial.println("Roller Task active!");

  while (true) {
    timeClient.update();
    //if (pir->detectedMotion() && timeClient.getHours() >= 8 && timeClient.getHours() <= 19) {
    if (pir->detectedMotion() && timeClient.getMinutes() % 2 == 0) {
      mqttClient.publish("smartroom/roller", "up");
      vTaskDelete(rollerTaskHandle);
    //} else if (!pir->detectedMotion() && (timeClient.getHours() < 8 || timeClient.getHours() > 19)) {
    } else if (!pir->detectedMotion() && timeClient.getMinutes() % 2 != 0) {
      mqttClient.publish("smartroom/roller", "down");
      vTaskDelete(rollerTaskHandle);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void lightTask(void* pvParameters) {
  (void)pvParameters;

  Serial.println("Light Task active!");

  while (true) {
    if (pir->detectedMotion() && lightSensor->getIntensity() < LIGHT_THRESHOLD) {
      if (! (lastLightState == 1)){
        mqttClient.publish("smartroom/light", "on");
        lastLightState = 1;
      }
    } else if ((!pir->detectedMotion())) {
      if (! (lastLightState == 0)){
        mqttClient.publish("smartroom/light", "off");
        lastLightState = 0;
      }
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Handle MQTT message received
  // This function is called when a message is received on a subscribed topic

  // Convert payload to a string
  char receivedMessage[length + 1];
  memcpy(receivedMessage, payload, length);
  receivedMessage[length] = '\0';

  // Print received message
  Serial.print("Message received on topic: ");
  Serial.print(topic);
  Serial.print("  ");
  Serial.println(receivedMessage);

  if (String(topic) == "smartroom/espEnabled") {
    if (String(receivedMessage) == "true") {

      //activate task if not activated
      if (xTaskGetHandle("Light Task") == NULL) {
        xTaskCreatePinnedToCore(lightTask, "Light Task", 4096, NULL, 1, &lightTaskHandle, 1);
      }

      xSemaphoreTake(auto_manual_mutex, portMAX_DELAY);
      isAutoEnable = true;
      xSemaphoreGive(auto_manual_mutex);
      led->switchOn();

    } else if (String(receivedMessage) == "false") {
      //deactivate task if activated
      if (xTaskGetHandle("Roller Task") != NULL) {
        vTaskDelete(rollerTaskHandle);
      }
      if (xTaskGetHandle("Light Task") != NULL) {
        vTaskDelete(lightTaskHandle);
      }

      xSemaphoreTake(auto_manual_mutex, portMAX_DELAY);
      lastLightState = -1;
      isAutoEnable = false;
      xSemaphoreGive(auto_manual_mutex);
      led->switchOff();
    }
  }
}


void reconnectWiFi(){
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    led->switchOff();
    delay(200);
    led->switchOn();
    delay(200);
  }
  Serial.print("WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMqtt() {
  // Loop until we're reconnected to the MQTT broker
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect(mqttClientId, mqttUsername, mqttPassword)) {
      mqttClient.subscribe("smartroom/roller");
      mqttClient.subscribe("smartroom/light");
      mqttClient.subscribe("smartroom/espEnabled");
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("MQTT connection failed with error code: ");
      Serial.println(mqttClient.state());
      led->switchOff();
      delay(200);
      led->switchOn();
      delay(200);
    }
  }
}
