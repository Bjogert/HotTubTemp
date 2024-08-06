# HotTubTemp

## Overview
The `HotTubTemp` project is designed to monitor the temperature of a hot tub and display the readings on an OLED screen. The system comprises two main components: a sender module and a receiver module. The sender module measures the temperature using a DS18B20 temperature sensor and transmits the data wirelessly using ESP-NOW. The receiver module receives the temperature data and displays it on an SSD1306 OLED screen.

## Components
### Sender Module
- **Microcontroller**: ESP8266
- **Temperature Sensor**: DS18B20
- **Libraries**:
  - `ESP8266WiFi`
  - `espnow`
  - `OneWire`
  - `DallasTemperature`

### Receiver Module
- **Microcontroller**: ESP8266
- **Display**: SSD1306 OLED
- **Libraries**:
  - `ESP8266WiFi`
  - `espnow`
  - `Wire`
  - `Adafruit GFX`
  - `Adafruit SSD1306`

## Repository Structure
HotTubTemp/
├── hotTubReceiver/
│ └── hotTubReceiver.ino
├── hotTubSender/
│ └── hotTubSender.ino
└── README.md


## Installation and Setup
### Prerequisites
- Arduino IDE
- Git
- Required Libraries:
  - Install the necessary libraries through the Arduino Library Manager.

### Cloning the Repository
1. Clone the repository to your local machine:
   ```sh
   git clone https://github.com/Bjogert/HotTubTemp.git
   
2. Navigate to the project directory
   cd HotTubTemp
3.
-Setting Up the Sender Module
Open the hotTubSender.ino file in the Arduino IDE.
Ensure the DS18B20 sensor is connected to the correct GPIO pin.
Upload the code to your ESP8266 board.

-Setting Up the Receiver Module
Open the hotTubReceiver.ino file in the Arduino IDE.
Ensure the OLED display is connected correctly.
Upload the code to your ESP8266 board.

4. Usage
-Power on both the sender and receiver modules.
The sender will read the temperature and send it to the receiver.
The receiver will display the temperature on the OLED screen.

5. Troubleshooting
- Ensure both ESP8266 modules are powered and within range.
Verify the MAC addresses in the code match the actual hardware.
Check the wiring connections for both the DS18B20 sensor and the OLED display.

Now you can enjoy a "lagom" hot bath!
