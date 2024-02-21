#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN D5     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11
#define Light_ADDR A0;   // address:0x23
#define PORT 3000

DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "Martin";
const char *password = "87651234";
const char *serverUrl = "147.45.103.103"; // Замените на адрес вашего сервера

void setup() {
  Serial.begin(115200);

  // Initialize the display
  // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for (;;)
  //     ;
  // }

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
  delay(1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius 
  int light = analogRead(A0);
  int atmo = 0;

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
  Serial.print("Light: ");
  Serial.print(light);

  // Отправка данных на сервер
  sendDataToServer(t, h, light, atmo);
}

void sendDataToServer(float temperature, float humidity, int light, int atmo) {
  WiFiClient client;

  if (!client.connect(serverUrl, PORT)) {
    Serial.println("No connectio to server");
    return;
  }
  Serial.println("Connected to server");


  String url = "/weather";
  String data = "{\"temperature\":" + String(temperature, 2) + ", \"humidity\":" + String(humidity, 2) + ", \"light\":" + String(light) + ", \"pressure\":" + String(atmo) +  "}";

  // Отправка HTTP POST запроса на сервер
  client.println("POST " + url + " HTTP/1.1");
  client.println("Host: " + String(serverUrl));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(data.length()));
  client.println();
  client.print(data);

  Serial.println("Request sent to server");
  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  client.stop();
}
