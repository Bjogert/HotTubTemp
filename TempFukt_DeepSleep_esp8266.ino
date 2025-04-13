#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_sleep.h"  // Include the ESP32 sleep library

// Define I2C pins (adjust according to your board's pinout)
#define SDA_PIN 21  // Adjust this to match your board's SDA pin
#define SCL_PIN 22  // Adjust this to match your board's SCL pin

// Sleep time configuration
#define uS_TO_S_FACTOR 1000000  // Conversion factor for seconds to microseconds
#define TIME_TO_SLEEP 300          // Sleep time in seconds (adjust as needed)

// Wi-Fi credentials
const char* ssid = "Vilda Webben extended";        // Your Wi-Fi SSID
const char* password = "SamMoa123";                // Your Wi-Fi Password

// MQTT broker settings
const char* mqtt_server = "192.168.68.105";        // MQTT Broker IP
const int mqtt_port = 1883;                        // Default MQTT port
const char* mqtt_user = "wiesel";                  // MQTT Username
const char* mqtt_password = "JuliaRobert2739";     // MQTT Password

// Unique identifier for this sensor
const char* sensor_id = "6";  // Change this number for each sensor

// MQTT topics (include the unique sensor ID)
char temperature_topic[50];
char humidity_topic[50];
char client_id[30];

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until reconnected to MQTT broker
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect with MQTT credentials
    if (client.connect(client_id, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to topics here if needed
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("; retrying in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize I2C with specified SDA and SCL pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize sensor with Wire instance only
  if (!htu.begin(&Wire)) {
    Serial.println("Couldn't find HTU21 sensor!");
    // Enter deep sleep if sensor initialization fails
    esp_deep_sleep_start();
  } else {
    Serial.println("HTU21 sensor initialized successfully!");
  }

  // Prepare unique MQTT topics and client ID
  snprintf(temperature_topic, sizeof(temperature_topic), "home/esp32_%s/temperature", sensor_id);
  snprintf(humidity_topic, sizeof(humidity_topic), "home/esp32_%s/humidity", sensor_id);
  snprintf(client_id, sizeof(client_id), "ESP32_TempSensor_%s", sensor_id);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor values
  float temperature = htu.readTemperature();
  float humidity = htu.readHumidity();

  // Check if readings are valid
  if (!isnan(temperature) && !isnan(humidity)) {
    // Convert readings to strings
    char tempStr[8];
    dtostrf(temperature, 6, 2, tempStr);
    char humStr[8];
    dtostrf(humidity, 6, 2, humStr);

    // Publish to MQTT topics
    client.publish(temperature_topic, tempStr);
    client.publish(humidity_topic, humStr);

    // Print readings to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(tempStr);
    Serial.print(" °C, Humidity: ");
    Serial.print(humStr);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read from HTU21 sensor!");
  }

  // Allow time for MQTT messages to be sent
  delay(100);  // Short delay to ensure messages are sent

  // Disconnect MQTT and Wi-Fi to save power
  client.disconnect();
  WiFi.disconnect(true);

  // Configure the wake-up timer
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Going to sleep now.");
  Serial.flush();  // Ensure all Serial data is sent before sleeping
  esp_deep_sleep_start();

  // The ESP32 will restart from here after waking up
}
