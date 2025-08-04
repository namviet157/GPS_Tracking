#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// Wifi 
#include "Wifi.h"

#define WIFI_SSID "Public Network"
#define WIFI_PASSWORD "789789789"

// Firebase
#include "FirebaseESP32.h"

#define API_KEY "AIzaSyDMRQvPf9OZkhp1sO-WjvwW4kjRC07B3Ek"
#define DATABASE_URL "https://gps-test-68ee2-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

bool signupOk = false;

void initFirebase() {

  firebaseConfig.api_key = API_KEY;
  firebaseConfig.database_url = DATABASE_URL;

  // Đồng bộ NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  // Đợi thời gian đồng bộ xong
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for NTP time sync...");
    delay(1000);
  }

  // Tiến hành đăng ký
  if (Firebase.signUp(&firebaseConfig, &firebaseAuth, "", "")) {
    Serial.println("Sign up Ok");
    signupOk = true;
  } else {
    Serial.println(firebaseConfig.signer.signupError.message.c_str());
    return;
  }

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

// NEO 6M
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

#define CARD_ID " 91 67 4E 09"
#define CHIP_ID " F2 68 FC 1D"

MFRC522 mfrc522(SS_PIN, RST_PIN);

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

// Program
bool alert = false;
bool auto_warning = false;
bool alert_ring = false;

bool last_alert = false;
bool last_alertRing = false;
bool last_auto = false;

unsigned long lastCheck = 0;
const unsigned long checkInterval = 3000; 

unsigned long pre_time_vibrate = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // WIFI
  Serial.print("Connecting to wifi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected success");
  
  // Firebase
  initFirebase();

  // GPS NEO 6M
  GPS_Serial.begin(9600, SERIAL_8N1, TX_NEO, RX_NEO);

  // RFID RC522
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init();

  // LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  // BUTTON
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);

  // BUZZER
  pinMode(BUZZER, OUTPUT);

  // SRF04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // SW520D
  pinMode(SW520D_PIN, INPUT);
}

long getDistance() {
  // Kích xung siêu âm
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Đo thời gian phản hồi (timeout 30ms ~ tương đương 5m)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Nếu không có phản hồi
  if (duration == 0) {
    return -1; // Không đo được
  }

  // Tính khoảng cách (cm)
  long distanceCm = duration * 0.0343 / 2;
  return distanceCm;
}

String dump_byte_array(byte *buffer, byte bufferSize) {
  String content = "";
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    content.concat(String(buffer[i] < 0x10 ? " 0" : " "));
    content.concat(String(buffer[i], HEX));
  }
  content.toUpperCase();
  return content;
}

void checkVibrate() {
  if (analogRead(SW520D_PIN) < 3000) {
    if (pre_time_vibrate == 0) {
      pre_time_vibrate = millis(); // bắt đầu đếm thời gian
    } else if (millis() - pre_time_vibrate >= 2000) { // sau 10 giây
      alert_ring = true;
      if (Firebase.setBool(firebaseData, "/alertRing", alert_ring)) {
        Serial.println("Dữ liệu đã gửi!");
      } else {
        Serial.println("khong gui duc");
      }
      Serial.println("Rung liên tục 2 giây -> Cảnh báo!");
    }
  } else {
    pre_time_vibrate = 0; // reset nếu không còn rung
  }
}

void loop() {
    if (Firebase.ready() && signupOk && millis() - lastCheck > checkInterval) {
    lastCheck = millis();

    // Đọc dữ liệu từ Firebase
    if (Firebase.getBool(firebaseData, "/auto")) {
      auto_warning = firebaseData.boolData();
    }

    if (Firebase.getBool(firebaseData, "/alert")) {
      alert = firebaseData.boolData();
    }

    if (Firebase.getBool(firebaseData, "/alertRing")) {
      alert_ring = firebaseData.boolData();
    }
  }

  // Cập nhật thiết bị theo trạng thái
  digitalWrite(LED_GREEN, alert);
  digitalWrite(LED_YELLOW, auto_warning);
  digitalWrite(BUZZER, alert_ring);
  digitalWrite(LED_RED, alert_ring);

  // Nút chuyển trạng thái auto_warning
  if (digitalRead(BUTTON_2) == HIGH) {
    auto_warning = !auto_warning;

    if (auto_warning != last_auto) {
      if (Firebase.setBool(firebaseData, "/auto", auto_warning)) {
        Serial.println("Đã gửi trạng thái auto_warning!");
        last_auto = auto_warning;
      }
    }

    digitalWrite(LED_YELLOW, auto_warning);
    delay(300); // chống bấm nhầm
  }

  // Nút tắt cảnh báo
  if (digitalRead(BUTTON_1) == HIGH) {
    alert_ring = false;
    auto_warning = false;

    digitalWrite(BUZZER, alert_ring);
    digitalWrite(LED_RED, alert_ring);
    digitalWrite(LED_YELLOW, auto_warning);

    if (alert_ring != last_alertRing) {
      if (Firebase.setBool(firebaseData, "/alertRing", alert_ring)) {
        Serial.println("Tắt alertRing!");
        last_alertRing = alert_ring;
      }
    }

    if (auto_warning != last_auto) {
      if (Firebase.setBool(firebaseData, "/auto", auto_warning)) {
        Serial.println("Tắt auto_warning!");
        last_auto = auto_warning;
      }
    }

    delay(300); // chống bấm nhầm
  }

  // Nếu ở chế độ auto và xe rời xa
  if (auto_warning && getDistance() > DISTANCE) {
    alert = true;
    if (alert != last_alert) {
      if (Firebase.setBool(firebaseData, "/alert", alert)) {
        Serial.println("Xe bị đưa đi xa!");
        last_alert = alert;
      }
    }
  }

  // Nếu có cảnh báo và rung động xảy ra
  if (alert && analogRead(SW520D_PIN) < 2048) {
    alert_ring = true;
    if (alert_ring != last_alertRing) {
      if (Firebase.setBool(firebaseData, "/alertRing", alert_ring)) {
        Serial.println("Có rung động khi cảnh báo!");
        last_alertRing = alert_ring;
      }
    }
  }

  // Bật còi khi đang cảnh báo
  if (alert_ring) {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED_RED, HIGH);
  }
  // if (Firebase.ready() && signupOk) {

  //   // Đọc dữ liệu
  //   if (Firebase.getString(firebaseData, "/auto")) {
  //     auto_warning = firebaseData.boolData();
  //   }

  //   if (Firebase.getString(firebaseData, "/alert")) {
  //     alert = firebaseData.boolData();
  //   }

  //   if (Firebase.getString(firebaseData, "/alertRing")) {
  //     alert_ring = firebaseData.boolData();
  //   }
  // }

  
  // digitalWrite(LED_GREEN, alert);
  // digitalWrite(LED_YELLOW, auto_warning);
  // digitalWrite(BUZZER, alert_ring);
  // digitalWrite(LED_RED, alert_ring);

  // if(digitalRead(BUTTON_2) == HIGH) {
  //   auto_warning = !auto_warning;
  //   if (Firebase.setBool(firebaseData, "/auto", auto_warning)) {
  //     Serial.println("Dữ liệu đã gửi!");
  //   }
  //   digitalWrite(LED_YELLOW, auto_warning);
  // }

  // if(digitalRead(BUTTON_1) == HIGH) {
  //   alert_ring = false;
  //   auto_warning = false;
  //   digitalWrite(BUZZER, alert_ring);
  //   digitalWrite(LED_RED, alert_ring);
  //   digitalWrite(LED_YELLOW, auto_warning);
  //   if (Firebase.setBool(firebaseData, "/alertRing", alert_ring)) {
  //     Serial.println("Dữ liệu đã gửi!");
  //   }
  //   if (Firebase.setBool(firebaseData, "/auto", auto_warning)) {
  //     Serial.println("Dữ liệu đã gửi!");
  //   } else {
  //     Serial.println("khong gui duc");
  //   }
  // }

  // if(auto_warning == true && getDistance() > DISTANCE) {
  //   alert = true;
  //   if (Firebase.setBool(firebaseData, "/alert", alert)) {
  //     Serial.println("Dữ liệu đã gửi!");
  //   }
  // }

  // if(alert == true) {
  //   if(analogRead(SW520D_PIN) < 2048) {
  //     alert_ring = true;
  //     if (Firebase.setBool(firebaseData, "/alertRing", alert_ring)) {
  //       Serial.println("Dữ liệu đã gửi!");
  //     }
  //   }
  // }

  
  // if(alert_ring == true) {
  //   digitalWrite(BUZZER, HIGH);
  //   digitalWrite(LED_RED, HIGH);
  // }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print(F("Card UID:"));
    String uid = dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println(" ");
    Serial.print("UID: ");
    Serial.println(uid);

    if(uid == CARD_ID) {
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(BUZZER, HIGH);
      delay(500);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(BUZZER, LOW);
    }
    else {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(BUZZER, HIGH);
      delay(500);
      digitalWrite(LED_RED, LOW);
      digitalWrite(BUZZER, LOW);
    }

    mfrc522.PICC_HaltA();          
    mfrc522.PCD_StopCrypto1();
  }

  while (GPS_Serial.available()) {
    char c = GPS_Serial.read();
    gps.encode(c);

    if (gps.location.isValid() && gps.location.isUpdated()) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);

      Serial.print("Date: ");
      Serial.print(gps.date.day());
      Serial.print("/");
      Serial.print(gps.date.month());
      Serial.print("/");
      Serial.println(gps.date.year());

      Serial.print("Time (UTC+7): ");
      int hourVN = (gps.time.hour() + 7) % 24;
      Serial.print(hourVN);
      Serial.print(":");
      Serial.print(gps.time.minute());
      Serial.print(":");
      Serial.println(gps.time.second());

      Serial.println("--------------------------");
      Firebase.setFloat(firebaseData, "/posBike/lat", gps.location.lat());
      Firebase.setFloat(firebaseData, "/posBike/lon", gps.location.lng());
    }
  }

}