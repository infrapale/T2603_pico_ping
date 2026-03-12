#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // --- HTTP GET test ---
  HTTPClient http;

  // if (http.begin("http://time.is")) {   // HTTP only (no https)
  // if (http.begin("http://worldtimeapi.org/api/ip.txt")) {
  //if (http.begin("http://timeapi.io/api/Time/current/zone?timeZone=Europe/Helsinki")) {
  //if (http.begin("http://timeapi.io/api/Time/current/zone?timeZone=Etc/UTC&format=txt")) {
  //if (http.begin("http://worldclockapi.com/api/json/utc/now")) {
  if (http.begin("http://example.com")) {

    int code = http.GET();

    if (code > 0) {
      Serial.print("HTTP OK, code = ");
      Serial.println(code);

      String payload = http.getString();
      Serial.println("Response:");
      Serial.println(payload);
    } else {
      Serial.print("HTTP GET failed, error = ");
      Serial.println(code);
    }

    http.end();
  } else {
    Serial.println("HTTP begin() failed");
  }
}

void loop() {}