#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>

// --- WiFi Configuration ---
const char* ssid = "TelstraA6C2A3";
const char* password = "bj35mguku9";

// --- MQTT Configuration ---
const char* mqtt_server = "2baa1f76a52a43eebf493cfa6af00c74.s1.eu.hivemq.cloud";
#define MQTT_PORT 8883
#define MQTT_USER "JackC"
#define MQTT_PASSWORD "Check1122"
#define MQTT_TOPIC "room/light/status"

// --- WiFi and MQTT Clients ---
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);

// --- Sensor Pins ---
const int pirPin = 2;  // PIR motion sensor

// --- Light Sensor ---
BH1750 lightMeter;

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");

    if (mqttClient.connect("ArduinoNano33Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed. MQTT state: ");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 4 seconds...");
      delay(4000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pirPin, INPUT);    // Set motion sensor pin as input
  Wire.begin();              // Start I2C for light sensor
  lightMeter.begin();        // Initialize light sensor

  connectToWiFi();

  mqttClient.setServer(mqtt_server, MQTT_PORT);
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }

  mqttClient.loop();

  // --- Read Sensors ---
  int motionDetected = digitalRead(pirPin);
  float lightLevel = lightMeter.readLightLevel();

  // --- Print sensor data ---
  if (motionDetected) {
    Serial.println("Motion detected!");
  } else {
    Serial.println("No motion.");
  }

  Serial.print("Light level: ");
  Serial.print(lightLevel);
  Serial.println(" lux");

  // --- Send data to MQTT ---
  char payload[100];
  snprintf(payload, sizeof(payload), "{\"motion\":%d,\"lux\":%.2f}", motionDetected, lightLevel);
  
  mqttClient.publish(MQTT_TOPIC, payload);
  Serial.print("MQTT sent: ");
  Serial.println(payload);

  delay(4000);  // Pause for 4 seconds
}
