#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include "secrets.h"

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ThingSpeak channel details
unsigned long myChannelNumber = 1234567;  // Replace with your ThingSpeak Channel Number
const char* myWriteAPIKey = "YOUR_WRITE_API_KEY"; // Replace with your Write API Key

WiFiClient client;

// Trig and Echo pins for 3 sensors
const int trigPins[] = {D1, D3, D5};
const int echoPins[] = {D2, D4, D6};
const int numSlots = 3;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  ThingSpeak.begin(client);

  // Set pin modes
  for (int i = 0; i < numSlots; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
}

// Function to get distance from a specific sensor
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void loop() {
  for (int i = 0; i < numSlots; i++) {
    float distance = getDistance(trigPins[i], echoPins[i]);
    int carDetected = (distance < 10) ? 1 : 0;

    Serial.print("Slot ");
    Serial.print(i + 1);
    Serial.print(" - Distance: ");
    Serial.print(distance);
    Serial.print(" cm | Car: ");
    Serial.println(carDetected);

    // Send to ThingSpeak field (field1 = Slot 1, field2 = Slot 2, ...)
    ThingSpeak.writeField(myChannelNumber, i + 1, carDetected, myWriteAPIKey);
    delay(1000);  // Wait to avoid overlap between requests
  }

  delay(15000); // Main delay between batches
}
