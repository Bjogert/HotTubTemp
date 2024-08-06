#include <ESP8266WiFi.h>
#include <espnow.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xAC, 0x0B, 0xFB, 0xD9, 0x6E, 0xF2};

// Data structure to send temperature
typedef struct struct_message {
  float temperature;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 2;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
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

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Start the DS18B20 sensor
  sensors.begin();
}
 
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Request temperature from DS18B20 sensor
    sensors.requestTemperatures();

    // Read temperature from DS18B20 sensor
    float temperatureC = sensors.getTempCByIndex(0);

    // Set temperature value to send
    myData.temperature = temperatureC;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    lastTime = millis();
  }
}