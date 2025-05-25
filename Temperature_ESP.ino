#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Firebase
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
FirebaseData fbData;

// DS18B20
#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED không khởi động được"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  display.println("Dang ket noi WiFi...");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  display.clearDisplay();
  display.println("Da ket noi WiFi!");
  display.display();
  delay(1000);

  // Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // DS18B20
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // Ghi lên Serial
  Serial.print("Nhiet do: ");
  Serial.print(temperature);
  Serial.println(" C");

  // Hiển thị OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Nhiet do: ");
  display.print(temperature);
  display.println(" *C");
  display.display();

  // Gửi dữ liệu lên Firebase
  if (Firebase.setFloat(fbData, "/Temperature", temperature)) {
    Serial.println("Cap nhat Firebase thanh cong");
  } else {
    Serial.print("Firebase error: ");
    Serial.println(fbData.errorReason());
  }

  delay(5000); // mỗi 5s cập nhật
}
