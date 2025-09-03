#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

#define DHTPIN 4       // GPIO pin connected to DATA
#define DHTTYPE DHT11  // DHT11 or DHT22

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

void reconnect() {
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
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);  // RabbitMQ MQTT port
  client.setKeepAlive(60);              // Set keepalive interval (in seconds)
}

void outputReadings() {
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

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

  Serial.print("Temp: ");
  Serial.print(correctedTempC);
  Serial.print(" °C / ");
  Serial.print(tempF);
  Serial.print(" °F | Humidity: ");
  Serial.print(correctedHumidity);
  Serial.println(" %");

  delay(2000);
}
