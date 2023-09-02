#include <SPI.h>
#include <WiFiNINA.h>

char wifiNetwork[] = "Sanchit";
char wifiPass[] = "SanchitGupta";

const char* iftttApiKey = "n6WS--n42J-Ucfj9zAiB3qu110gqwgAmMAhXya5og_4";
const char* eventSunlightStart = "Sunlight_Detected";
const char* eventSunlightStop = "Sunlight_Ended";

const int lightSensorPin = A0;
bool isSunlightDetected = false;

WiFiClient wifiClient;

void setup() {
  Serial.begin(9600);

  while (WiFi.begin(wifiNetwork, wifiPass) != WL_CONNECTED) {
    delay(1000);
    Serial.println("Attempting to establish the WiFi connection");
  }
  Serial.println("WiFi connection established");
}

void loop() {
  int lightIntensity = analogRead(lightSensorPin);

  if (!isSunlightDetected && lightIntensity < 700) {
    sendIFTTTEvent(eventSunlightStart);
    isSunlightDetected = true;
  } else if (isSunlightDetected && lightIntensity > 700) {
    sendIFTTTEvent(eventSunlightStop);
    isSunlightDetected = false;
  }

  delay(1500);  
}

void sendIFTTTEvent(const char* eventName) {
  if (wifiClient.connect("maker.ifttt.com", 80)) {
    Serial.println("Connected to IFTTT");

    String requestURL = "/trigger/";
    requestURL += eventName;
    requestURL += "/with/key/";
    requestURL += iftttApiKey;

    wifiClient.println("POST " + requestURL + " HTTP/1.1");
    wifiClient.println("Host: maker.ifttt.com");
    wifiClient.println("Connection: close\r\n\r\n");
    delay(10);

    while (wifiClient.available()) {
      char receivedChar = wifiClient.read();
      Serial.print(receivedChar);
    }
    Serial.println();
    wifiClient.stop();
    Serial.println("IFTTT event sent");
  } else {
    Serial.println("Unable to send the event");
  }
}