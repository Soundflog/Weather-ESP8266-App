#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";
const char *serverUrl = "YOUR_SERVER_URL"; // Замените на адрес вашего сервера

void setup() {
  Serial.begin(115200);

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to the WiFi network");

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print temperature and humidity
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  // Отправка данных на сервер
  sendDataToServer(t, h);
}

void sendDataToServer(float temperature, float humidity) {
  WiFiClient client;

  if (!client.connect(serverUrl, 80)) {
    Serial.println("Connection failed");
    return;
  }

  Serial.println("Connected to server");

  String url = "/weather";
  String data = "temperature=" + String(temperature) + "&humidity=" + String(humidity);

  // Отправка HTTP POST запроса на сервер
  client.println("POST " + url + " HTTP/1.1");
  client.println("Host: " + String(serverUrl));
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: " + String(data.length()));
  client.println();
  client.print(data);

  Serial.println("Request sent to server");
  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.println(line);
    }
  }

  client.stop();
}
