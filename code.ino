#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>          // Thư viện I2C
#include <BH1750.h>        // Thư viện BH1750 (Cần có thư viện BH1750)
#include <DHT.h>
#include <ESP8266HTTPClient.h>  // Thư viện HTTP Client để gửi POST

// Khai báo các chân kết nối
#define DHTPIN D4           // Chân kết nối của cảm biến DHT11
#define DHTTYPE DHT11       // Định nghĩa cảm biến DHT11

// Khai báo chân LED điều khiển cho đèn, quạt, điều hòa
#define LED_LIGHT_PIN D1  // Chân GPIO điều khiển đèn
#define LED_FAN_PIN D2    // Chân GPIO điều khiển quạt
#define LED_AC_PIN D3     // Chân GPIO điều khiển điều hòa

// Khai báo thông tin WiFi, MQTT và API
const char* ssid = "PTIT_WIFI";        // Tên mạng Wi-Fi
const char* password = "";      // Mật khẩu Wi-Fi
const char* mqtt_server = "10.21.194.50";   // Địa chỉ IP của MQTT broker
const char* serverName = "http://10.21.194.50:5501/api/sensor";  // Endpoint API

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

// Khởi tạo cảm biến ánh sáng BH1750
BH1750 lightMeter(0x23);  // Cảm biến với địa chỉ 0x23

void setup() {
  Serial.begin(9600);
  dht.begin();  // Khởi động cảm biến DHT

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  // Kết nối MQTT broker
  client.setServer(mqtt_server, 1885);  // Chú ý điều chỉnh port nếu cần
  client.setCallback(callback);

  // Cài đặt chân LED là OUTPUT
  pinMode(LED_LIGHT_PIN, OUTPUT);
  pinMode(LED_FAN_PIN, OUTPUT);
  pinMode(LED_AC_PIN, OUTPUT);

  // Mặc định tắt các thiết bị
  digitalWrite(LED_LIGHT_PIN, LOW);
  digitalWrite(LED_FAN_PIN, LOW);
  digitalWrite(LED_AC_PIN, LOW);

  // Khởi tạo I2C và cảm biến BH1750
  Wire.begin(D6, D5);  // Khởi tạo I2C với SDA trên D6 (GPIO12) và SCL trên D5 (GPIO14)
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Light sensor BH1750 started successfully");
  } else {
    Serial.println("Error starting BH1750 sensor");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (WiFi.status() == WL_CONNECTED) {  // Kiểm tra kết nối WiFi
    HTTPClient http;

    // Đọc dữ liệu từ cảm biến DHT
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Đọc giá trị ánh sáng từ cảm biến BH1750
    float lux = lightMeter.readLightLevel();

    // Kiểm tra lỗi khi đọc cảm biến DHT
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      // Nếu đọc thành công, in ra giá trị
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" °C, Humidity: ");
      Serial.print(humidity);
      Serial.print(" %, Light: ");
      Serial.print(lux);
      Serial.println(" lx");

      // Gửi dữ liệu tới MQTT broker
      String payload = String("{\"temperature\":") + temperature +
                       ",\"humidity\":" + humidity + 
                       ",\"light\":" + lux + "}";
      client.publish("sensor/data", payload.c_str());  // Gửi dữ liệu tới MQTT broker

      // Chuẩn bị dữ liệu JSON để gửi lên API
      String jsonData = String("{\"tem\":") + temperature + 
                        ",\"hum\":" + humidity + 
                        ",\"light\":" + lux + "}";

      // Gửi dữ liệu qua HTTP POST đến API
      WiFiClient httpClient;  // Tạo một WiFiClient mới
      http.begin(httpClient, serverName);  // Thiết lập URL cho request với WiFiClient
      http.addHeader("Content-Type", "application/json");  // Gán Header JSON
      int httpResponseCode = http.POST(jsonData);  // Gửi POST request
      
      // Kiểm tra kết quả
      if (httpResponseCode > 0) {
        String response = http.getString();  // Lấy phản hồi từ server
        Serial.println(httpResponseCode);  // In ra mã phản hồi
        Serial.println(response);  // In ra phản hồi
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();  // Đóng kết nối HTTP
    }
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(2000);  // Gửi dữ liệu mỗi 2 giây
}

// Hàm gửi trạng thái thiết bị qua API
void sendDeviceState(const String& deviceName, const String& state) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient httpClient;
    
    // Chuẩn bị dữ liệu JSON để gửi lên API
    String jsonData = String("{\"device_name\":\"") + deviceName + "\",\"state\":\"" + state + "\"}";

    http.begin(httpClient, "http://10.21.194.50:5501/api/device"); // Thay đổi endpoint API nếu cần
    http.addHeader("Content-Type", "application/json");  // Gán Header JSON
    int httpResponseCode = http.POST(jsonData);  // Gửi POST request

    // Kiểm tra kết quả
    if (httpResponseCode > 0) {
      String response = http.getString();  // Lấy phản hồi từ server
      Serial.println(httpResponseCode);  // In ra mã phản hồi
      Serial.println(response);  // In ra phản hồi
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Đóng kết nối HTTP
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Hàm xử lý khi nhận lệnh từ MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Điều khiển thiết bị theo lệnh nhận từ MQTT
  if (String(topic) == "control/light") {
    if (message == "ON") {
      digitalWrite(LED_LIGHT_PIN, HIGH);  // Bật đèn
      sendDeviceState("light", "ON"); // Gửi trạng thái lên API
    } else if (message == "OFF") {
      digitalWrite(LED_LIGHT_PIN, LOW);   // Tắt đèn
      sendDeviceState("light", "OFF"); // Gửi trạng thái lên API
    }
  }

  // Tương tự cho quạt và điều hòa
  if (String(topic) == "control/fan") {
    if (message == "ON") {
      digitalWrite(LED_FAN_PIN, HIGH);    // Bật quạt
      sendDeviceState("fan", "ON"); // Gửi trạng thái lên API
    } else if (message == "OFF") {
      digitalWrite(LED_FAN_PIN, LOW);     // Tắt quạt
      sendDeviceState("fan", "OFF"); // Gửi trạng thái lên API
    }
  }

  if (String(topic) == "control/ac") {
    if (message == "ON") {
      digitalWrite(LED_AC_PIN, HIGH);     // Bật điều hòa
      sendDeviceState("ac", "ON"); // Gửi trạng thái lên API
    } else if (message == "OFF") {
      digitalWrite(LED_AC_PIN, LOW);      // Tắt điều hòa
      sendDeviceState("ac", "OFF"); // Gửi trạng thái lên API
    }
  }

  // Điều khiển tất cả thiết bị
  if (String(topic) == "control/all") {
    if (message == "ON") {
      digitalWrite(LED_LIGHT_PIN, HIGH);  // Bật đèn
      digitalWrite(LED_FAN_PIN, HIGH);    // Bật quạt
      digitalWrite(LED_AC_PIN, HIGH);     // Bật điều hòa
      sendDeviceState("all", "ON"); // Gửi trạng thái lên API
    } else if (message == "OFF") {
      digitalWrite(LED_LIGHT_PIN, LOW);   // Tắt đèn
      digitalWrite(LED_FAN_PIN, LOW);     // Tắt quạt
      digitalWrite(LED_AC_PIN, LOW);      // Tắt điều hòa
      sendDeviceState("all", "OFF"); // Gửi trạng thái lên API
    }
  }
}

// Hàm kết nối lại với MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", "bach", "123")) {  // Thay đổi nếu cần username và password
      Serial.println("connected");
      client.subscribe("control/#");  // Đăng ký lắng nghe các topic điều khiển
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
