#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Skrillex";
const char* password = "69996969";
const char* serverUrl = "http://192.168.209.49:4000/";

int thermistorPin = A2;
int Vo;
float c1 = 0.001129148;
float c2 = 0.000234125;
float c3 = 0.0000000876741;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected.");
}

void loop() {
  // Read the voltage from the thermistor
  Vo = analogRead(thermistorPin); // read raw ADC value
  // float voltage = Vo*(5.0/4095.0); // convert raw value to voltage (ESP32 has a 12-bit ADC)
  float resistance = 10000*(4095.0/(float)Vo - 1.0); // calculates resistance of thermistor in ohms(10K pull-up resistor)
  float temp = (1.0/(c1 + (c2*log(resistance)) + c3*log(resistance)*log(resistance)*log(resistance))); // calculates temperature in Kelvin using the Steinhart-Hart equation
  temp-=273.15; // converts to Celsius

  // Create the HTTP client object
  HTTPClient http;

  // Construct the data to send
  // String data = String(temp, 2);

  StaticJsonDocument<200> doc;
  doc["id"] = 1;
  doc["smoke"] = temp;
  String data;
  serializeJson(doc, data);

  // Send the HTTP POST request
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(data);

  // Send the HTTP POST request
  // http.begin(serverUrl);
  // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // int httpCode = http.POST(data);
  http.end();

  // Print the result
  Serial.print("Smoke: ");
  Serial.print(temp, 2);
  Serial.print("C, HTTP code: ");
  Serial.println(httpCode);

  // Wait for 5 seconds before sending the next request
  delay(5000);
}
