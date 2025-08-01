#include <EEPROM.h> //Tên wifi và mật khẩu lưu vào ô nhớ 0->96
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h> //Thêm thư viện web server
WebServer webServer(80); //Khởi tạo đối tượng webServer port 80
#include <Ticker.h>
#include <esp_event.h>
Ticker blinker;

String ssid = "";
String password = "";
#define ledPin 2
#define btnPin 0
unsigned long lastTimePress=millis();
#define PUSHTIME 5000
unsigned long blinkTime=millis();
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

void blinkLed(uint32_t t){
  if(millis()-blinkTime>t){
    digitalWrite(ledPin,!digitalRead(ledPin));
    blinkTime=millis();
  }
}

void ledControl(){
  if(digitalRead(btnPin)==LOW){
    if(millis()-lastTimePress<PUSHTIME){
      blinkLed(1000);
    }else{
      blinkLed(50);
    }
  }else{
    blinkLed(50);
  }
}

//Chương trình xử lý sự kiện wifi
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:  //Gửi thông tin về PC khi kết nối wifi
      Serial.println("Connected to WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: //Tự kết nối lại khi mất wifi
      Serial.println("Disconnected from WiFi");
      WiFi.begin(ssid, password);
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
    // WiFi.onEvent(WiFiEvent); //Đăng ký chương trình bắt sự kiện wifi
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
  webServer.on("/",[]{
    webServer.send(200, "text/html", html); //Gửi nội dung HTML
  });
  webServer.on("/scanWifi",[]{
    Serial.println("Scanning wifi network...!");
    int wifi_nets = WiFi.scanNetworks(true, true);
    const unsigned long t = millis();
    while(wifi_nets<0 && millis()-t<10000){
      delay(20);
      wifi_nets = WiFi.scanComplete();
    }
    DynamicJsonDocument doc(200);
    for(int i=0; i<wifi_nets; ++i){
      Serial.println(WiFi.SSID(i));
      doc.add(WiFi.SSID(i));
    }
    //["tên wifi1","tên wifi2","tên wifi3",.....]
    String wifiList = "";
    serializeJson(doc, wifiList);
    Serial.println("Wifi list: "+wifiList);
    webServer.send(200,"application/json",wifiList);
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
  webServer.on("/resetWifi",[]{
    // Xóa thông tin WiFi đã lưu
    for(int i=0; i<100; i++){
      EEPROM.write(i,0);
    }
    EEPROM.commit();
    ssid = "";
    password = "";
    webServer.send(200,"text/plain","WiFi information has been reset!");
  });
  webServer.on("/reStart",[]{
    webServer.send(200,"text/plain","Esp32 is restarting!");
    delay(3000);
    ESP.restart();
  });
  webServer.begin(); //Khởi chạy dịch vụ web server trên ESP32
}

void checkButton(){
  if(digitalRead(btnPin)==LOW){
    Serial.println("Press and hold for 5 seconds to reset to default!");
    if(millis()-lastTimePress>PUSHTIME){
      for(int i=0; i<100;i++){
        EEPROM.write(i,0);
      }
      EEPROM.commit();
      Serial.println("EEPROM memory erased!");
      delay(2000);
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
    pinMode(ledPin,OUTPUT);
    pinMode(btnPin,INPUT_PULLUP);
    blinker.attach_ms(50, ledControl);
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