# ESP32 MQTT Weather Station
This project uses an ESP32 microcontroller paired with a DHT11 sensor to periodically measure temperature and humidity. The device connects to WiFi, publishes sensor data as a JSON payload via MQTT, and routes it through RabbitMQ for backend processing. To conserve battery, the ESP32 enters deep sleep between transmissions, waking every 5–10 minutes to repeat the cycle.

The backend is designed to consume messages using .NET microservices, enabling structured logging, dashboard integration, and scalable data handling. The sy

## ESP32 Sensor Lifecycle with MQTT and RabbitMQ
This project uses an ESP32 microcontroller paired with a DHT11 sensor to periodically capture temperature and humidity data, publish it via MQTT, and route it through RabbitMQ for backend processing. The system is optimized for battery efficiency using deep sleep cycles.

### Execution Flow
1. **Wake from Deep Sleep**
The ESP32 wakes every 5 minutes using its RTC timer. Boot count is tracked for diagnostics.

2. **Initialize Components**
WiFi and MQTT connections are established. The DHT11 sensor is powered and initialized.

3. **Read Sensor Data**
Temperature and humidity are captured. Readings are validated to ensure accuracy.

4. **Publish Payload**
A JSON payload is formatted and published to a designated MQTT topic (e.g., sensor/dht11). RabbitMQ routes the message to a bound queue for downstream consumption.

5. **Disconnect and Sleep**
WiFi and MQTT connections are closed. The ESP32 enters deep sleep to conserve battery until the next wake cycle.

This architecture supports modular backend integration, including .NET microservices that consume RabbitMQ messages, log telemetry, and trigger alerts or dashboards. The system is designed for low-power, real-time telemetry in edge environments.