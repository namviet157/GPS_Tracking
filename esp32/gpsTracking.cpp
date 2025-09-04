#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#include "Wifi.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <WebServer.h>
WebServer webServer(80);
#include <Ticker.h>
#include <esp_event.h>
#include "FirebaseESP32.h"

#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"

FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

bool signupOk = false;

void initFirebase() {
  firebaseConfig.api_key = API_KEY;
  firebaseConfig.database_url = DATABASE_URL;

  // Đồng bộ NTP
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");

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
  Firebase.reconnectWiFi(false);
}

// NEO 6M
#define TX_NEO 16
#define RX_NEO 17

TinyGPSPlus gps;
HardwareSerial GPS_Serial(1);

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

// LED
#define LED_RED 13
#define LED_GREEN 12
#define LED_YELLOW 14

// SW520D
#define SW520D_PIN 35

// BUTTON
#define BUTTON_1 32 // RED
#define BUTTON_2 33 // YELLOW

// BUZZER
#define BUZZER 27

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


// change this to make the song slower or faster
int tempo = 100;

// change this to whichever pin you want to use

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  NOTE_D4,4, NOTE_FS4,8, NOTE_G4,8, NOTE_A4,4, NOTE_FS4,8, NOTE_G4,8, 
  NOTE_A4,4, NOTE_B3,8, NOTE_CS4,8, NOTE_D4,8, NOTE_E4,8, NOTE_FS4,8, NOTE_G4,8, 
  NOTE_FS4,4, NOTE_D4,8, NOTE_E4,8, NOTE_FS4,4, NOTE_FS3,8, NOTE_G3,8,
  NOTE_A3,8, NOTE_G3,8, NOTE_FS3,8, NOTE_G3,8, NOTE_A3,2,
  NOTE_G3,4, NOTE_B3,8, NOTE_A3,8, NOTE_G3,4, NOTE_FS3,8, NOTE_E3,8, 
  NOTE_FS3,4, NOTE_D3,8, NOTE_E3,8, NOTE_FS3,8, NOTE_G3,8, NOTE_A3,8, NOTE_B3,8
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

void led(bool red, bool yellow, bool green){
  digitalWrite(LED_RED, red);
  digitalWrite(LED_YELLOW, yellow);
  digitalWrite(LED_GREEN, green);
}

void playMelody() {
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  bool red = false;
  bool yellow = false;
  bool green = false;
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER, melody[thisNote], noteDuration * 0.9);

    led(red, yellow, green);
    red = !red;
    yellow = !yellow;
    green = !green;

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(BUZZER);
  }
  
}

String ssid = "";
String password = "";
unsigned long lastTimePress=millis();
//Tạo biến chứa mã nguồn trang web HTML để hiển thị trên trình duyệt
const char html[] PROGMEM = R"html( 
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>BikeGuard - WiFi Setup</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #ecfdf5 0%, #ffffff 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
            overflow: hidden;
            max-width: 500px;
            width: 100%;
            animation: slideIn 0.5s ease-out;
        }
        
        @keyframes slideIn {
            from {
                opacity: 0;
                transform: translateY(20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        .header {
            background: linear-gradient(135deg, #059669 0%, #047857 100%);
            padding: 30px;
            text-align: center;
            color: white;
        }
        
        .header-icon {
            width: 60px;
            height: 60px;
            background: rgba(255, 255, 255, 0.2);
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            margin: 0 auto 15px;
            font-size: 24px;
        }
        
        .header h1 {
            font-size: 24px;
            font-weight: 700;
            margin-bottom: 5px;
        }
        
        .header p {
            opacity: 0.9;
            font-size: 14px;
        }
        
        .content {
            padding: 30px;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        .form-label {
            display: block;
            font-size: 14px;
            font-weight: 600;
            color: #374151;
            margin-bottom: 8px;
        }
        
        .form-input {
            width: 100%;
            padding: 12px 16px;
            border: 2px solid #e5e7eb;
            border-radius: 10px;
            font-size: 16px;
            transition: all 0.2s;
            background: #f9fafb;
        }
        
        .form-input:focus {
            outline: none;
            border-color: #059669;
            background: white;
            box-shadow: 0 0 0 3px rgba(5, 150, 105, 0.1);
        }
        
        .form-select {
            width: 100%;
            padding: 12px 16px;
            border: 2px solid #e5e7eb;
            border-radius: 10px;
            font-size: 16px;
            transition: all 0.2s;
            background: #f9fafb;
            cursor: pointer;
        }
        
        .form-select:focus {
            outline: none;
            border-color: #059669;
            background: white;
            box-shadow: 0 0 0 3px rgba(5, 150, 105, 0.1);
        }
        
        .status-message {
            text-align: center;
            padding: 15px;
            margin-bottom: 20px;
            border-radius: 10px;
            font-size: 14px;
            font-weight: 500;
        }
        
        .status-scanning {
            background: #fef3c7;
            color: #92400e;
            border: 1px solid #fde68a;
        }
        
        .status-complete {
            background: #d1fae5;
            color: #065f46;
            border: 1px solid #a7f3d0;
        }
        
        .button-group {
            display: flex;
            gap: 12px;
            margin-top: 30px;
        }
        
        .btn {
            flex: 1;
            padding: 14px 20px;
            border: none;
            border-radius: 10px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }
        
        .btn-primary {
            background: #059669;
            color: white;
        }
        
        .btn-primary:hover {
            background: #047857;
            transform: translateY(-1px);
            box-shadow: 0 4px 12px rgba(5, 150, 105, 0.3);
        }
        
        .btn-secondary {
            background: #f3f4f6;
            color: #374151;
        }
        
        .btn-secondary:hover {
            background: #e5e7eb;
            transform: translateY(-1px);
        }
        
        .loading {
            display: inline-block;
            width: 16px;
            height: 16px;
            border: 2px solid #ffffff;
            border-radius: 50%;
            border-top-color: transparent;
            animation: spin 1s linear infinite;
        }
        
        @keyframes spin {
            to {
                transform: rotate(360deg);
            }
        }
        
        .icon {
            width: 16px;
            height: 16px;
            fill: currentColor;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="header-icon">📶</div>
            <h1>WiFi Configuration</h1>
            <p>Connect your BikeGuard device to WiFi</p>
        </div>
        
        <div class="content">
            <div id="status" class="status-message status-scanning">
                <span class="loading"></span>
                Scanning WiFi networks...
            </div>
            
            <div class="form-group">
                <label for="ssid" class="form-label">WiFi Network</label>
                <select id="ssid" class="form-select">
                    <option value="">Select WiFi network...</option>
                </select>
            </div>
            
            <div class="form-group">
                <label for="password" class="form-label">WiFi Password</label>
                <input type="password" id="password" class="form-input" placeholder="Enter WiFi password">
            </div>
            
            <div class="button-group">
                <button onclick="saveWifi()" class="btn btn-primary">
                    <svg class="icon" viewBox="0 0 24 24">
                        <path d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z"/>
                    </svg>
                    Save Configuration
                </button>
                <button onclick="reStart()" class="btn btn-secondary">
                    <svg class="icon" viewBox="0 0 24 24">
                        <path d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15"/>
                    </svg>
                    Restart
                </button>
            </div>
        </div>
    </div>

    <script>
        window.onload = function() {
            scanWifi();
        }
        
        var xhttp = new XMLHttpRequest();
        
        function scanWifi() {
            xhttp.onreadystatechange = function() {
                if (xhttp.readyState == 4 && xhttp.status == 200) {
                    data = xhttp.responseText;
                    document.getElementById("status").innerHTML = "✅ WiFi scan completed!";
                    document.getElementById("status").className = "status-message status-complete";
                    
                    var obj = JSON.parse(data);
                    var select = document.getElementById("ssid");
                    
                    // Clear existing options except the first one
                    select.innerHTML = '<option value="">Select WiFi network...</option>';
                    
                    for (var i = 0; i < obj.length; ++i) {
                        select[select.length] = new Option(obj[i], obj[i]);
                    }
                }
            }
            xhttp.open("GET", "/scanWifi", true);
            xhttp.send();
        }
        
        function saveWifi() {
            var ssid = document.getElementById("ssid").value;
            var pass = document.getElementById("password").value;
            
            if (!ssid) {
                alert("Please select a WiFi network");
                return;
            }
            
            if (!pass) {
                alert("Please enter the WiFi password");
                return;
            }
            
            // Show loading state
            var saveBtn = event.target;
            var originalText = saveBtn.innerHTML;
            saveBtn.innerHTML = '<span class="loading"></span> Saving...';
            saveBtn.disabled = true;
            
            xhttp.onreadystatechange = function() {
                if (xhttp.readyState == 4 && xhttp.status == 200) {
                    data = xhttp.responseText;
                    alert(data);
                    
                    // Reset button
                    saveBtn.innerHTML = originalText;
                    saveBtn.disabled = false;
                }
            }
            xhttp.open("GET", "/saveWifi?ssid=" + encodeURIComponent(ssid) + "&pass=" + encodeURIComponent(pass), true);
            xhttp.send();
        }
        
        function reStart() {
            var restartBtn = event.target;
            var originalText = restartBtn.innerHTML;
            restartBtn.innerHTML = '<span class="loading"></span> Restarting...';
            restartBtn.disabled = true;
            
            xhttp.onreadystatechange = function() {
                if (xhttp.readyState == 4 && xhttp.status == 200) {
                    data = xhttp.responseText;
                    alert(data);
                    
                    // Reset button
                    restartBtn.innerHTML = originalText;
                    restartBtn.disabled = false;
                }
            }
            xhttp.open("GET", "/reStart", true);
            xhttp.send();
        }
    </script>
</body>
</html>
)html";

//Chương trình xử lý sự kiện wifi
int wifiRetryCount = 0;
const int maxWifiRetries = 3;

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:  //Gửi thông tin về PC khi kết nối wifi
      Serial.println("Connected to WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      wifiRetryCount = 0; // Reset retry count on successful connection
      playMelody();
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: //Tự kết nối lại khi mất wifi
      Serial.println("Disconnected from WiFi");
      wifiRetryCount++;
      if (wifiRetryCount >= maxWifiRetries) {
        Serial.println("Failed to connect after 3 attempts. Clearing WiFi credentials.");
        for (int i = 0; i < 100; i++) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();
        ssid = "";
        password = "";
        wifiRetryCount = 0;
        Serial.println("WiFi credentials erased. Please connect to a new WiFi network.");
        led(true, true, true);
        tone(BUZZER, 1000, 2000);
        delay(2000);
        led(false, false, false);
        tone(BUZZER, 0, 0);
        ESP.restart();
      } else {
        WiFi.begin(ssid, password);
      }
      break;
    default:
      break;
  }
}

void setupWifi(){
  if(ssid!=""){
    Serial.println("Connecting to wifi...!");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WiFiEvent, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  }else{
    Serial.println("ESP32 wifi network created!");
    WiFi.mode(WIFI_AP);
    uint8_t macAddr[6];
    WiFi.softAPmacAddress(macAddr);
    String ssid_ap="ESP32-"+String(macAddr[4],HEX)+String(macAddr[5],HEX);
    ssid_ap.toUpperCase();
    WiFi.softAP(ssid_ap.c_str());
    Serial.println("Access point name:"+ssid_ap);
    Serial.println("Web server access address:"+WiFi.softAPIP().toString());
  }
}

void setupWebServer(){
  //Thiết lập xử lý các yêu cầu từ client(trình duyệt web)
  webServer.on("/", HTTP_GET, []{
    webServer.send(200, "text/html", html); //Gửi nội dung HTML
  });
  webServer.on("/scanWifi", HTTP_GET, []{
    Serial.println("Scanning wifi network...!");
    int wifi_nets = WiFi.scanNetworks(true, true);
    const unsigned long t = millis();
    while(wifi_nets<0 && millis()-t<10000){
      delay(20);
      wifi_nets = WiFi.scanComplete();
    }
    DynamicJsonDocument doc(1024);
    for(int i=0; i<wifi_nets; ++i){
      Serial.println(WiFi.SSID(i));
      doc.add(WiFi.SSID(i));
    }
    //["tên wifi1","tên wifi2","tên wifi3",.....]
    String wifiList = "";
    serializeJson(doc, wifiList);
    Serial.println("Wifi list: "+wifiList);
    webServer.send(200,"application/json",wifiList);
    WiFi.scanDelete(); 
  });
  webServer.on("/saveWifi",[]{
    String ssid_temp = webServer.arg("ssid");
    String password_temp = webServer.arg("pass");
    Serial.println("SSID:"+ssid_temp);
    Serial.println("PASS:"+password_temp);
    EEPROM.writeString(0,ssid_temp);
    EEPROM.writeString(32,password_temp);
    EEPROM.commit();
    webServer.send(200,"text/plain","Wifi has been saved!");
  });
  webServer.on("/reStart",[]{
    webServer.send(200,"text/plain","Esp32 is restarting!");
    delay(3000);
    ESP.restart();
  });
  webServer.onNotFound([](){
    webServer.send(404, "text/plain", "Not Found");
  });
  webServer.begin(); //Khởi chạy dịch vụ web server trên ESP32
}

void checkButton(){
  if(digitalRead(BUTTON_1)==HIGH && digitalRead(BUTTON_2)==HIGH){
    Serial.println("Press and hold for 5 seconds to reset to default!");
    if(millis()-lastTimePress>5000){
      for(int i=0; i<100;i++){
        EEPROM.write(i,0);
      }
      EEPROM.commit();
      Serial.println("EEPROM memory erased!");
      ssid = "";
      password = "";
      led(true, true, true);
      tone(BUZZER, 1000, 2000);
      delay(2000);
      led(false, false, false);
      tone(BUZZER, 0, 0);
      ESP.restart();
    }
    delay(1000);
  }else{
    lastTimePress=millis();
  }
}

class Config{
public:
  void begin(){
    EEPROM.begin(100);
    ssid = EEPROM.readString(0);
    password = EEPROM.readString(32);
    if(ssid!=""){
      Serial.println("Wifi name:"+ssid);
      Serial.println("Password:"+password);
    }
    setupWifi(); //Thiết lập wifi
    setupWebServer(); //Luôn thiết lập webserver để có thể truy cập
  }
  void run(){
    checkButton();
    webServer.handleClient(); //Luôn lắng nghe yêu cầu từ client
  }
} wifiConfig;

// Program
bool alert = false;
bool auto_warning = false;
bool alert_ring = false;
bool comfirm_card = false;

bool last_alert = false;
bool last_alertRing = false;
bool last_auto = false;
bool vibrating = false;

int countVibration = 0;

unsigned long lastCheck = 0;
const unsigned long checkInterval = 500; 

unsigned long pre_time_vibrate = millis();

float lastLat = 0.0;
float lastLon = 0.0;
unsigned long lastGPSSend = 0;
const unsigned long gpsSendInterval = 10000;
const float gpsDeltaThreshold = 0.0001; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // GPS NEO 6M
  GPS_Serial.begin(9600, SERIAL_8N1, RX_NEO, TX_NEO);

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

  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER, 0);

  wifiConfig.begin();

  // Firebase
  initFirebase();
}

long getDistance();

String dump_byte_array(byte *buffer, byte bufferSize);

void checkVibrate();

void updateGPS();

void sendData();

void getData();

void sendVibrationData();

void loop() {
  wifiConfig.run();
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    getData();
  }
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print(F("Card UID:"));
    String uid = dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println(" ");
    Serial.print("UID: ");
    Serial.println(uid);

    if(uid == CARD_ID) {
      comfirm_card = !comfirm_card;
      digitalWrite(LED_GREEN, comfirm_card);
      digitalWrite(BUZZER, HIGH);
      delay(500);
      digitalWrite(BUZZER, LOW);
    }
    else {
      comfirm_card = false;
      digitalWrite(LED_GREEN, comfirm_card);
      digitalWrite(LED_RED, HIGH);
      digitalWrite(BUZZER, HIGH);
      delay(500);
      digitalWrite(LED_RED, LOW);
      digitalWrite(BUZZER, LOW);
    }
    mfrc522.PICC_HaltA();          
    mfrc522.PCD_StopCrypto1();
  }

  if (digitalRead(BUTTON_2) == HIGH) {
    if(comfirm_card == true) {
      auto_warning = !auto_warning;
      alert = false;
    }
    else {
      auto_warning = true;
    }
  }

  if (digitalRead(BUTTON_1) == HIGH) {
    if(comfirm_card == true) {
      alert_ring = false;
    }
  }

  if(auto_warning) {
    if(comfirm_card && getDistance() > DISTANCE) {
      alert = true;
      comfirm_card = false;
    }
    else if(comfirm_card == false) {
      alert = true;
    }
  }

  if (alert) {
    if (digitalRead(SW520D_PIN) == LOW) { 
      if (!vibrating) {
        vibrating = true;
        pre_time_vibrate = millis();
      }
  
      Serial.println("Rung");
      countVibration++;
      Serial.println(countVibration);
  
      if (millis() - pre_time_vibrate >= 5000) {
        sendVibrationData();
        countVibration = 0;
        vibrating = false;
      }
    } 
  }

  if(alert_ring) {
    digitalWrite(BUZZER, alert_ring);
    digitalWrite(LED_RED, alert_ring);
  }

  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    sendData();
  }

  updateGPS();
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
    } 
    else if (millis() - pre_time_vibrate >= 2000) { // sau 10 giây
      alert_ring = true;
      if (WiFi.status() == WL_CONNECTED && Firebase.ready() && Firebase.setBool(firebaseData, "/alertRing", alert_ring)) {
        Serial.println("Dữ liệu đã gửi!");
      } else {
        Serial.println("khong gui duoc");
      }
      Serial.println("Rung liên tục 2 giây -> Cảnh báo!");
    }
  } else {
    pre_time_vibrate = 0; // reset nếu không còn rung
  }
}

void updateGPS() {
  while (GPS_Serial.available()) {
    char c = GPS_Serial.read();
    gps.encode(c);
  }

  if (gps.location.isValid() && gps.location.isUpdated()) {
    float curLat = gps.location.lat();
    float curLon = gps.location.lng();
    bool moved = abs(curLat - lastLat) > gpsDeltaThreshold || abs(curLon - lastLon) > gpsDeltaThreshold;
    bool timeElapsed = millis() - lastGPSSend > gpsSendInterval;

    if (moved || timeElapsed) {
      Serial.print("Latitude: "); Serial.println(curLat, 6);
      Serial.print("Longitude: "); Serial.println(curLon, 6);
      Serial.print("Date: "); Serial.print(gps.date.day()); Serial.print("/"); Serial.print(gps.date.month()); Serial.print("/"); Serial.println(gps.date.year());
      Serial.print("Time (UTC+7): ");
      int hourVN = (gps.time.hour() + 7) % 24;
      Serial.print(hourVN); Serial.print(":"); Serial.print(gps.time.minute()); Serial.print(":"); Serial.println(gps.time.second());
      Serial.println("--------------------------");

      if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
        Firebase.setFloat(firebaseData, "/posBike/lat", curLat);
        Firebase.setFloat(firebaseData, "/posBike/lon", curLon);
      }

      lastLat = curLat;
      lastLon = curLon;
      lastGPSSend = millis();
    }
  }
}

void sendData() {
  if(alert != last_alert) {  
    if (Firebase.setBool(firebaseData, "/alert", alert)) {
      Serial.println("Gui alert");
    }
  }

  if(auto_warning != last_auto) {  
    if (Firebase.setBool(firebaseData, "/auto", auto_warning)) {
      Serial.println("Gui auto_warning");
    }
  }

  if(alert_ring != last_alertRing) {  
    if (Firebase.setBool(firebaseData, "/alertRing", alert_ring)) {
      Serial.println("Gui alert_ring");
    }
  }
}

void getData() {
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

    last_auto = auto_warning;
    last_alert = alert;
    last_alertRing = alert_ring;
  }

  // Cập nhật thiết bị theo trạng thái
  digitalWrite(LED_GREEN, comfirm_card);
  digitalWrite(LED_YELLOW, auto_warning || alert);
  digitalWrite(BUZZER, alert_ring);
  digitalWrite(LED_RED, alert_ring);
}

void sendVibrationData() {
  alert_ring = true;
  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    FirebaseJson vibrationJson;

    String timeStr = "";
    if (gps.time.isValid()) {
      int hourVN = (gps.time.hour() + 7) % 24;
      timeStr = (hourVN < 10 ? "0" : "") + String(hourVN) + ":" + (gps.time.minute() < 10 ? "0" : "") + String(gps.time.minute());
    }
    vibrationJson.set("hour", timeStr);
    vibrationJson.set("numberOfVibration", countVibration);

    if (Firebase.pushJSON(firebaseData, "/vibrationData", vibrationJson)) {
      Serial.println("Gửi thành công!");
    } else {
      Serial.println(firebaseData.errorReason());
    }
  }
}
