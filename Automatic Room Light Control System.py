import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import json

# Define relay GPIO pin
RELAY_PIN = 17

# Setup GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(RELAY_PIN, GPIO.OUT)

# Callback when connected to MQTT broker
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe("room/light/status")

# Callback when a message is received
def on_message(client, userdata, msg):
    
        data = json.loads(msg.payload.decode())
        motion = data.get("motion", 0)
        lux = data.get("lux", 1000)

        if motion == 1 and lux < 3000:
            GPIO.output(RELAY_PIN, GPIO.HIGH)
            print("Relay ON (Light ON)")
        else:
            GPIO.output(RELAY_PIN, GPIO.LOW)
            print("Relay OFF (Light OFF)")

# Create an MQTT client and the connection
client = mqtt.Client()
client.username_pw_set("JackC", "Check1122")
client.tls_set()  
client.on_connect = on_connect
client.on_message = on_message

# Connect to MQTT broker
broker_url = "2baa1f76a52a43eebf493cfa6af00c74.s1.eu.hivemq.cloud"
client.connect(broker_url, 8883)

# Start MQTT loop
client.loop_forever()
