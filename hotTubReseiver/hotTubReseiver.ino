#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// REPLACE WITH SENDER MAC Address
uint8_t senderMAC[] = {0xA4, 0xCF, 0x12, 0xC1, 0xC3, 0x82};

// Structure example to receive data
typedef struct struct_message {
  float temperature;
} struct_message;

// Create a struct_message called receivedData
struct_message receivedData;

static unsigned long lastReceiveTime = 0; // Last time a message was received

// Callback when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Received temperature: ");
  Serial.print(receivedData.temperature);
  Serial.println(" °C");
  lastReceiveTime = millis(); // Update last receive time
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function when data is received
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  esp_now_add_peer(senderMAC, ESP_NOW_ROLE_CONTROLLER, 1, NULL, 0);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
}

void loop() {
  // Check if data is received and update display
  if (millis() - lastReceiveTime <= 5000) {
    display.clearDisplay();
    display.setTextSize(2.7);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.print("Badtunnan:");
    display.setTextSize(3); // Change the text size to 3, equivalent to 30 pixels in height
    display.print(receivedData.temperature);
    display.print((char)247); // degree symbol
    display.print("C");
    display.display();
  } else {
    display.clearDisplay();
    display.setTextSize(2); // Smaller text size
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.print("No Data");
    display.display();
  }

  delay(1000);
}
