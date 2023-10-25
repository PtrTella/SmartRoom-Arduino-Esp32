# Copyright 2021 HiveMQ GmbH
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import paho.mqtt.client as mqtt
from dashboard_server import add_light_value, add_roller_value
from arduinoh import serial_set_light, serial_set_roller_blind

# Topics
roller_control_topic = "smartroom/roller"
light_control_topic = "smartroom/light"
esp_board_topic = "smartroom/espEnabled"

# Callback when connected to the MQTT broker
def on_connect(client, userdata, flags, rc, properties=None):
    print("Connected to MQTT broker with result code " + str(rc))
    client.subscribe(roller_control_topic)
    client.subscribe(light_control_topic)
    client.subscribe(esp_board_topic)

# with this callback you can see if your publish was successful
def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

# print which topic was subscribed to
def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# Callback when a message is received from the MQTT broker
def on_message(client, userdata, msg):
    message = msg.payload.decode("utf-8")
    print("Received message: " + message)

    if msg.topic == roller_control_topic:
        if message == "up":
            add_roller_value(100)
            serial_set_roller_blind(100)
        elif message == "down":
            add_roller_value(0)
            serial_set_roller_blind(0)
    elif msg.topic == light_control_topic:
        add_light_value(message)
        serial_set_light(message)

# Initialize MQTT client
# using MQTT version 5 here, for 3.1.1: MQTTv311, 3.1: MQTTv31
client = mqtt.Client(client_id="SmartRoomClient", userdata=None, protocol=mqtt.MQTTv5)
# setting callbacks, use separate functions like above for better visibility
client.on_connect = on_connect
client.on_message = on_message
client.on_subscribe = on_subscribe
client.on_publish = on_publish

# enable TLS for secure connection
client.tls_set(tls_version=mqtt.ssl.PROTOCOL_TLS)
# set username and password
client.username_pw_set("ESP32", "Assignament2023")
# connect to HiveMQ Cloud on port 8883 (default for MQTT)
client.connect("78fbd30dbdf04400a7aefb4b5ee16a5d.s1.eu.hivemq.cloud", 8883)

# Start the MQTT loop, loop_forever need to stop the loop manually
client.loop_forever()



