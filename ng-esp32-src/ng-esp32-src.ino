#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "arduino_secrets.h"

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* url = SECRET_URL;

unsigned long previousMillis = 0;
const long interval = 2000; // 30 seconds
const int tiltSensorPin = 15; // GPIO4
int tiltSensorState = 0;
int counter = 0;
int previousTiltSensorState = tiltSensorState;

#define MIC_ANALOG_PIN 36  // A0 pin of the microphone, connected to GPIO36 (VP)
#define MIC_DIGITAL_PIN 13 // D0 pin of the microphone
const int sampleRate = 8000; // 8kHz sample rate
const int bufferSize = 5 * sampleRate;
uint8_t buffer[bufferSize];


void setup() {
  pinMode(MIC_ANALOG_PIN, INPUT);
  pinMode(MIC_DIGITAL_PIN, INPUT);
  
  adc1_config_width(ADC_WIDTH_BIT_10); // 10-bit ADC resolution
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // Attenuation to read up to 3.3V
  

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
        recordSound();
        uploadToServer();
      }
      previousTiltSensorState = HIGH;
  } else {
    previousTiltSensorState = LOW;
    Serial.printf("%d - No tilt detected.\n", counter);
  }
  counter++;
  delay(500); // Add a small delay to avoid flooding the serial monitor with messages
}


void recordSound() {
  Serial.println("Recording...");

  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = adc1_get_raw(ADC1_CHANNEL_0);
    delayMicroseconds(125); // 1 second / sampleRate
  }
  
  Serial.println("Recording finished.");
  Serial.printf("Last byte was %d", buffer[8000]);
  // Buffer now contains the recorded sound data
}

void uploadToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(url);
    http.addHeader("Content-Type", "application/octet-stream");

    int httpResponseCode = http.PUT(buffer, bufferSize);
    
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}
