#include <WiFi.h>
extern "C" {
#include "esp_wpa2.h"
}

const char* ssid = getEnv("wifi_ssid");
const char* username = getEnv("wifi_uname");
const char* password = getEnv("wifi_pswd");

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
  esp_wifi_sta_wpa2_ent_enable(&config);

  WiFi.begin(ssid);

  Serial.println("\\nConnecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  
  Serial.println("\\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {}
