const int tiltSensorPin = 15; // GPIO4
int tiltSensorState = 0;
int counter = 0;

void setup() {
  // ...
  Serial.begin(115200);
  pinMode(tiltSensorPin, INPUT); // Set the tilt sensor pin as an input
  Serial.println("Done setup");
}


void loop() {
  // ...
  tiltSensorState = digitalRead(tiltSensorPin); // Read the tilt sensor's output

  if (tiltSensorState == HIGH) {
    Serial.printf("%d - Tilt detected!\n", counter);
  } else {
    Serial.printf("%d - No tilt detected.\n", counter);
  }
  counter++;
  delay(500); // Add a small delay to avoid flooding the serial monitor with messages
}
