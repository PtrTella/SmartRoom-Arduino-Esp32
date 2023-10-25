from flask import *
from flask_cors import CORS
from flask_mqtt import Mqtt
from datetime import datetime
from arduinoh import serial_set_light, serial_set_roller_blind, serial_in
import threading
import time
import ssl
import os


separator = os.path.sep
app = Flask(__name__, template_folder=f"..{separator}room-dashboard{separator}templates", static_folder=f"..{separator}room-dashboard{separator}static")
CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'

lock = threading.Lock()


#---------------------------------DATA-----------------------------#

def get_time():
    return str(datetime.now())

data = {
    'LIGHT_VALUES': [{
        'state': False,
        'ts': get_time()
    }],
    'ROLLER_BLIND_VALUES': [{
       'state': 0,
        'ts': get_time()
    }]
}

def add_light_value(msg):
    if (msg == 'on'):
        data['LIGHT_VALUES'].append({
            'state': True,
            'ts': get_time()
        })
    elif (msg == 'off'):
        data['LIGHT_VALUES'].append({
           'state': False,
            'ts': get_time()
        })

def add_roller_value(msg):
    data['ROLLER_BLIND_VALUES'].append({
        'state': msg,
        'ts': get_time()
    })

#---------------------------------MQTT-----------------------------#

# Mqtt configuration
app.config['MQTT_BROKER_URL'] = '78fbd30dbdf04400a7aefb4b5ee16a5d.s1.eu.hivemq.cloud'
app.config['MQTT_BROKER_PORT'] = 8883
app.config['MQTT_KEEPALIVE'] = 60  # Set KeepAlive time in seconds
app.config['MQTT_TLS_ENABLED'] = True
app.config['MQTT_TLS_INSECURE'] = True
app.config['MQTT_TLS_VERSION'] = ssl.PROTOCOL_TLS
app.config['MQTT_CLIENT_ID'] = 'SmartRoomClient'
app.config['MQTT_USERNAME'] = 'ESP32'
app.config['MQTT_PASSWORD'] = 'Assignament2023'

# Topics
roller_control_topic = "smartroom/roller"
light_control_topic = "smartroom/light"
esp_board_topic = "smartroom/espEnabled"
mqtt = Mqtt(app)

ESP_enabled = True
ESP_threshold = datetime.now()

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker with result code " + str(rc))
    mqtt.subscribe(roller_control_topic)
    mqtt.subscribe(light_control_topic)
    mqtt.subscribe(esp_board_topic)

@mqtt.on_message()
def handle_mqtt_message(client, userdata, msg):
    global ESP_enabled
    message = msg.payload.decode("utf-8")
    print("Received message: " + message)

    lock.acquire()
    if ESP_enabled:
        if msg.topic == roller_control_topic:
            if message == "up":
                serial_set_roller_blind(100)
            elif message == "down":
                print("in down")
                serial_set_roller_blind(0)

        elif msg.topic == light_control_topic:
            serial_set_light(message)
    lock.release()




#-------------------------------FLASK - HTTP-------------------------------#

@app.route('/')
def dashboard():
    return render_template('dashboard.html')

@app.route('/light-chart-values')
def send_light_values():
    lock.acquire()
    result = jsonify(data['LIGHT_VALUES'])
    lock.release()
    return result

@app.route('/roller-blind-chart-values')
def send_roller_blind_values():
    lock.acquire()
    result = jsonify(data['ROLLER_BLIND_VALUES'])
    lock.release()
    return result

@app.route("/set-light", methods=["POST"])
def set_light():
    global ESP_enabled, ESP_threshold
    lock.acquire()
    if (mqtt.connected):
        print("Dashboard message arrived")
        changeEspStatus(False)
    val = request.data.decode()
    serial_set_light(val)
    lock.release()
    return Response(status = 200)

@app.route("/set-roller-blind", methods=["POST"])
def set_roller_blind():
    global ESP_enabled, ESP_threshold
    lock.acquire()
    if (mqtt.connected):
        print("Dashboard message arrived")
        changeEspStatus(False)
    position = int(request.data.decode())
    if (position < 0 or position > 180):
        return Response(status = 422)
    serial_set_roller_blind(position)
    lock.release()
    return Response(status = 200)



#-------------------------------SERIAL-------------------------------#

TIME_THRESHOLD = 30

#recive data from Arduino serial port
def update_data():
    global ESP_enabled, ESP_threshold
    while True:
        msg = serial_in()
        if msg != None:
            parse_serial_message(msg)
        
        if (not ESP_enabled) and mqtt.connected and (datetime.now() - ESP_threshold).total_seconds() > TIME_THRESHOLD:
            print("ESP reactivated!")
            changeEspStatus(True)            
        time.sleep(0.3)

def changeEspStatus(status):
    global ESP_enabled, ESP_threshold
    ESP_threshold = datetime.now()
    if(status == ESP_enabled):
        return
    else:
        ESP_enabled = status
        mqtt.publish(esp_board_topic, str(status).lower())


def parse_serial_message(msg):
    global ESP_enabled, ESP_threshold
    try:
        val = msg.split('\n')[:2]
        for i in val:
            key, value = i.split(':')
            value = value.strip()
            if (key == 'source' and mqtt.connected):
                if(value == 'bluetooth'):
                    print("BTmessage arrived")
                    changeEspStatus(False)
            elif (key == 'light'):
                add_light_value(value)
            elif (key == 'roller_blind'):
                add_roller_value(value)
    except ValueError as e:
        print(f"Error parsing message: {e}\n")


#--------------------------------------------------------------#

if __name__ == '__main__':
    app.debug = False
    app.run()

serial_reads_thread = threading.Thread(target=update_data)
serial_reads_thread.setDaemon(True)
serial_reads_thread.start()