# 🚗 BikeGuard - Hệ thống Chống trộm và Định vị Xe máy Thông minh

## 📋 Mô tả dự án

BikeGuard là một hệ thống IoT toàn diện được thiết kế để bảo vệ và theo dõi xe máy thông minh. Hệ thống bao gồm thiết bị phần cứng ESP32 tích hợp nhiều cảm biến, ứng dụng web frontend và backend API để quản lý người dùng và dữ liệu.

### ✨ Tính năng chính

- **🔒 Chống trộm thông minh**: Sử dụng RFID, cảm biến chuyển động và cảm biến khoảng cách
- **📍 Định vị GPS real-time**: Theo dõi vị trí xe máy theo thời gian thực
- **📱 Giao diện web responsive**: Dashboard quản lý trực quan và dễ sử dụng
- **🔔 Cảnh báo tức thì**: Thông báo khi phát hiện hành vi đáng ngờ
- **📊 Lưu trữ dữ liệu**: Lưu trữ lịch sử vị trí và sự kiện an ninh
- **👤 Quản lý người dùng**: Hệ thống đăng ký, đăng nhập và phân quyền

## 🏗️ Kiến trúc hệ thống

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   ESP32 Device  │    │  Web Frontend   │    │  Spring Boot    │
│                 │    │                 │    │    Backend      │
│ • GPS Module    │◄──►│ • Dashboard     │◄──►│ • REST API      │
│ • RFID Reader   │    │ • Authentication│    │ • User Mgmt     │
│ • Sensors       │    │ • Real-time Map │    │ • Database      │
│ • WiFi/GSM      │    │ • Alerts        │    │ • Security      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Firebase      │    │   Local Storage │    │   SQL Server    │
│   Realtime DB   │    │   (Browser)     │    │   Database      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## 📁 Cấu trúc dự án

```
GPS_Tracking/
├── esp32/                          # Phần cứng ESP32
│   └── gpsTracking.cpp             # Code chính cho ESP32
├── bikeguard-backend/              # Backend API
│   ├── src/main/java/
│   │   └── com/example/bikeguard_backend/
│   │       ├── controller/         # REST Controllers
│   │       ├── model/             # Data Models
│   │       ├── repository/        # Data Access Layer
│   │       └── service/           # Business Logic
│   ├── database/
│   │   └── insert_database.sql    # Database schema
│   └── pom.xml                    # Maven dependencies
└── web-frontend/                   # Frontend Web App
    ├── index.html                 # Trang chủ
    ├── auth.html                  # Trang đăng nhập/đăng ký
    ├── dashboard.html             # Dashboard chính
    └── assets/
        └── js/
            ├── firebase.js        # Firebase integration
            └── jquery-3.7.1.min.js # jQuery library
```

## 🛠️ Yêu cầu hệ thống

### Phần cứng ESP32
- **ESP32 Development Board**
- **GPS Module NEO-6M**
- **RFID Reader RC522**
- **Cảm biến khoảng cách SRF04 (HC-SR04)**
- **Cảm biến chuyển động SW520D**
- **LED indicators** (Đỏ, Xanh, Vàng)
- **Buzzer** cho cảnh báo âm thanh
- **Nút bấm** điều khiển
- **Antenna WiFi/GSM** (tùy chọn)

### Phần mềm
- **Java 21** hoặc cao hơn
- **Maven 3.6+**
- **SQL Server 2019+**
- **Arduino IDE** với ESP32 board support
- **Web browser** hiện đại (Chrome, Firefox, Safari, Edge)

## 🚀 Hướng dẫn cài đặt và chạy

### 1. Cài đặt Database

```sql
-- Chạy script SQL để tạo database
-- File: bikeguard-backend/database/insert_database.sql
```

### 2. Cấu hình Backend

1. **Clone repository:**
```bash
git clone <repository-url>
cd GPS_Tracking/bikeguard-backend
```

2. **Cấu hình database trong `application.properties`:**
```properties
spring.datasource.url=jdbc:sqlserver://localhost:1433;databaseName=YOUR_DB_NAME;encrypt=true;trustServerCertificate=true
spring.datasource.username=YOUR_DB_USERNAME
spring.datasource.password=YOUR_DB_PASSWORD
```

3. **Chạy ứng dụng Spring Boot:**
```bash
# Sử dụng Maven
mvn spring-boot:run

# Hoặc sử dụng Maven wrapper
./mvnw spring-boot:run
```

Backend sẽ chạy tại: `http://localhost:8081`

### 3. Cài đặt ESP32

1. **Cài đặt Arduino IDE và ESP32 board support**
2. **Cài đặt các thư viện cần thiết:**
   - MFRC522 (RFID)
   - TinyGPS++ (GPS)
   - ArduinoJson
   - FirebaseESP32
   - WebServer

3. **Cấu hình WiFi và Firebase (KHÔNG public secrets):**
```cpp
// Trong file gpsTracking.cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"
```

4. **Cấu hình Firebase cho Web Frontend (KHÔNG public secrets):**
```js
// Trong web-frontend/assets/js/firebase.js và web-frontend/dashboard.html
const firebaseConfig = {
  apiKey: "YOUR_FIREBASE_API_KEY",
  authDomain: "YOUR_FIREBASE_AUTH_DOMAIN",
  databaseURL: "YOUR_FIREBASE_DATABASE_URL",
  projectId: "YOUR_FIREBASE_PROJECT_ID",
  storageBucket: "YOUR_FIREBASE_STORAGE_BUCKET",
  messagingSenderId: "YOUR_FIREBASE_MESSAGING_SENDER_ID",
  appId: "YOUR_FIREBASE_APP_ID"
};
```

5. **Cấu hình dịch vụ thông báo (tùy chọn):**
```text
- PushSafer: đặt `YOUR_PUSHSAFER_KEY` trong `web-frontend/dashboard.html`
- EmailJS: đặt `YOUR_EMAILJS_PUBLIC_KEY`, `YOUR_EMAILJS_SERVICE_ID`, `YOUR_EMAILJS_TEMPLATE_ID` trong `web-frontend/dashboard.html`
```

4. **Upload code lên ESP32**

### 4. Chạy Web Frontend

1. **Mở file `web-frontend/index.html` trong trình duyệt**
2. **Hoặc sử dụng local server:**
```bash
cd web-frontend
# Sử dụng Python
python -m http.server 8000

# Hoặc sử dụng Node.js
npx http-server -p 8000
```

Frontend sẽ chạy tại: `http://localhost:8000`

Lưu ý bảo mật:
- Không commit API keys, mật khẩu, token, hoặc thông tin tài khoản lên repository công khai.
- Sử dụng biến môi trường hoặc file cấu hình cục bộ (không commit) để cung cấp secrets khi triển khai.
- Các ví dụ trong mã đã thay bằng placeholder như `YOUR_FIREBASE_API_KEY`.

## 📖 Hướng dẫn sử dụng

### Đăng ký tài khoản
1. Truy cập trang web: `http://localhost:8000`
2. Chọn "Đăng ký" và điền thông tin cá nhân
3. Xác nhận email (nếu có)

### Đăng nhập và sử dụng
1. Đăng nhập với email và mật khẩu
2. Truy cập Dashboard để xem:
   - Vị trí hiện tại của xe
   - Lịch sử di chuyển
   - Trạng thái cảm biến
   - Cảnh báo an ninh

### Cấu hình thiết bị
1. **Kết nối ESP32 với nguồn điện**
2. **Đảm bảo kết nối WiFi ổn định**
3. **Kiểm tra kết nối GPS và các cảm biến**

## 🔧 Cấu hình và tùy chỉnh

### Thay đổi cấu hình ESP32
```cpp
// Thay đổi chân kết nối cảm biến
#define TX_NEO 16      // GPS TX
#define RX_NEO 17      // GPS RX
#define SS_PIN 22      // RFID SS
#define RST_PIN 21     // RFID RST
#define TRIG_PIN 4     // Ultrasonic TRIG
#define ECHO_PIN 2     // Ultrasonic ECHO
```

### Tùy chỉnh cảnh báo
```cpp
// Thay đổi khoảng cách cảnh báo (cm)
#define DISTANCE 50

// Thay đổi thời gian gửi dữ liệu (ms)
#define SEND_INTERVAL 5000
```

### Cấu hình Firebase
1. Tạo project Firebase mới
2. Bật Realtime Database
3. Cập nhật API_KEY và DATABASE_URL trong code ESP32

## 📊 API Endpoints

### User Management
- `POST /api/users/register` - Đăng ký người dùng
- `POST /api/users/login` - Đăng nhập
- `GET /api/users/{id}` - Lấy thông tin người dùng
- `PUT /api/users/{id}` - Cập nhật thông tin người dùng

### GPS Data
- `GET /api/gps/location` - Lấy vị trí hiện tại
- `GET /api/gps/history` - Lấy lịch sử vị trí
- `POST /api/gps/update` - Cập nhật vị trí (từ ESP32)

⭐ Nếu dự án này hữu ích, hãy cho chúng tôi một star trên GitHub!



