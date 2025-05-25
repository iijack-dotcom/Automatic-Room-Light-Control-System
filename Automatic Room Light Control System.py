import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import json

# Relay pin connected to the light
RELAY_PIN = 17
LUX_THRESHOLD = 300  # Adjust based on your room lighting
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

        if lux >= LUX_THRESHOLD:
            GPIO.output(RELAY_PIN, GPIO.LOW)
            print("Light is bright, keep it OFF")
        elif motion == 1:
            GPIO.output(RELAY_PIN, GPIO.HIGH)
            print("Motion detected and dark → Light ON")
        else:
            GPIO.output(RELAY_PIN, GPIO.LOW)
            print("No motion or still dark → Light OFF")

# MQTT setup
client = mqtt.Client()
client.username_pw_set("JackC", "Check1122")
client.tls_set()

client.on_connect = on_connect
client.on_message = on_message

client.connect("2baa1f76a52a43eebf493cfa6af00c74.s1.eu.hivemq.cloud", 8883)
client.loop_forever()
