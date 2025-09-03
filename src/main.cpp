#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

#define DHTPIN 4       // GPIO pin connected to DATA
#define DHTTYPE DHT11  // DHT11 or DHT22
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60  // 5 minutes
#define LED_PIN 2

// RTC_DATA_ATTR int bootCount = 0; // counter

const char* mqtt_server = MQTT_SERVER;  // e.g. "192.168.1.100"

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

// Calibration offset
const float TEMP_OFFSET_C = -2.4;
const float HUMIDITY_OFFSET = 15.0;

void setup_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void reconnect_mqtt() {
  client.setServer(mqtt_server, 1883);  // RabbitMQ MQTT port
  client.setKeepAlive(60);

  while (!client.connected()) {
    if (client.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(client.state());
      delay(10000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  dht.begin();
  
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // blink awake
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);

  setup_wifi();
  reconnect_mqtt();

  float rawTempC = dht.readTemperature();
  float correctedTempC = rawTempC + TEMP_OFFSET_C;
  float tempF = correctedTempC * 1.8 + 32;
  float rawHumidity = dht.readHumidity();
  float correctedHumidity = rawHumidity + HUMIDITY_OFFSET;

  if (isnan(rawTempC) || isnan(rawHumidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } else {
    char payload[128];
    snprintf(payload, sizeof(payload), "{\"correctedTempC\":%.2f,\"tempF\":%.2f,\"correctedHumidity\":%.2f}", correctedTempC, tempF, correctedHumidity);
    client.publish("home/sensors/dht11", payload);
    Serial.println(payload);
  }

  
  client.disconnect();
  WiFi.disconnect();
  delay(500); // delay to finish flushing any remaining packets
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("LIGHT Sleeping for " + String(TIME_TO_SLEEP) + " seconds...");
  delay(500); // delay after waking
  esp_light_sleep_start();
  delay(500); // delay after waking



  
  Serial.println("Woke Up!");
}
