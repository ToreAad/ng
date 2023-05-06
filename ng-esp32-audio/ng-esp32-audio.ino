#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "arduino_secrets.h"

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* url = SECRET_URL;

#define BUTTON_PIN 0       // Built-in button pin
#define MIC_ANALOG_PIN 36  // A0 pin of the microphone, connected to GPIO36 (VP)
#define MIC_DIGITAL_PIN 13 // D0 pin of the microphone

const int sampleRate = 8000; // 8kHz sample rate
const int bufferSize = 5 * sampleRate;
uint8_t buffer[bufferSize];

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
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
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    recordSound();
    uploadToServer();
  }
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
