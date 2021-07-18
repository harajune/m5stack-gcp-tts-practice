#include <WiFi.h>
#include <M5Stack.h>

// WiFi settings
// define the macros below
// WIFI_SSID: WiFi SSID
// WIFI_PASS: WiFi Password
#include "credentials.h"

void setup() {
  M5.begin();
  Serial.begin(115200);

  Serial.println("Connecting to WiFi");

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting");
    delay(1000);
  }

  Serial.println("WiFi Connected");

}

void loop() {
  // put your main code here, to run repeatedly:
}