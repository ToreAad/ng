#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Galaxy S20 FE 5GDAAD";
const char* password = "sowt0928";
const char* url = "https://webhook.site/fa9e562d-4823-4a62-9e1a-d053d1bce90d";

unsigned long previousMillis = 0;
const long interval = 2000; // 30 seconds
const int tiltSensorPin = 15; // GPIO4
int tiltSensorState = 0;
int counter = 0;
int previousTiltSensorState = tiltSensorState;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(tiltSensorPin, INPUT); // Set the tilt sensor pin as an input
}

void loop() {
  // unsigned long currentMillis = millis();
  tiltSensorState = digitalRead(tiltSensorPin); // Read the tilt sensor's output

  if (tiltSensorState == HIGH) {
    Serial.printf("%d - Tilt detected!\n", counter);
      if (previousTiltSensorState == LOW) {
        // previousMillis = currentMillis;
        performGetRequest();
      }
      previousTiltSensorState = HIGH;
  } else {
    previousTiltSensorState = LOW;
    Serial.printf("%d - No tilt detected.\n", counter);
  }
  counter++;
  delay(500); // Add a small delay to avoid flooding the serial monitor with messages
}

void performGetRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Response:");
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
