const int tiltSensorPin = 15; // GPIO4
int tiltSensorState = 0;

void setup() {
  // ...
  Serial.begin(115200);
  pinMode(tiltSensorPin, INPUT); // Set the tilt sensor pin as an input
  Serial.println("Done setup");
}

void loop() {
  // ...
  Serial.println("Reading!");
  tiltSensorState = digitalRead(tiltSensorPin); // Read the tilt sensor's output

  if (tiltSensorState == HIGH) {
    Serial.println("Tilt detected!");
  } else {
    Serial.println("No tilt detected.");
  }

  delay(500); // Add a small delay to avoid flooding the serial monitor with messages
}
