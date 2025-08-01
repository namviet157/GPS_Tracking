#include "wifiConfig.h"

#define TX_NEO 16
#define RX_NEO 17

TinyGPSPlus gps;
HardwareSerial GPS_Serial(2);

// RFID RC522
#define SS_PIN 22 // SDA
#define RST_PIN 21 // RST

#define SCK_PIN 5
#define MISO_PIN 19
#define MOSI_PIN 23

// SRF04
#define TRIG_PIN 4
#define ECHO_PIN 2

#define DISTANCE 50

// BUZZER
#define BUZZER 27

// LED
#define LED_RED 13
#define LED_GREEN 12
#define LED_YELLOW 14

// SW520D
#define SW520D_PIN 35

// BUTTON
#define BUTTON_1 32 // RED
#define BUTTON_2 33 // YELLOW

void setup() {
  Serial.begin(115200);
  wifiConfig.begin();
}
void loop() {
  wifiConfig.run();
}