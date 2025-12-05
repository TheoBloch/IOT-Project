#include "task_core_iot.h"
#include "global.h"
#include <ArduinoJson.h>
#include "task_wifi.h"

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

constexpr char LED_STATE_ATTR[] = "ledState";

volatile int ledMode = 0;
volatile bool ledState = false;

constexpr uint16_t BLINKING_INTERVAL_MS_MIN = 10U;
constexpr uint16_t BLINKING_INTERVAL_MS_MAX = 60000U;
volatile uint16_t blinkingInterval = 1000U;

constexpr int16_t telemetrySendInterval = 10000U; // 10 seconds

constexpr std::array<const char *, 2U> SHARED_ATTRIBUTES_LIST = {
    LED_STATE_ATTR,
};

void processSharedAttributes(const Shared_Attribute_Data &data)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        // Reserved for future attributes
    }
}

RPC_Response setLedSwitchValue(const RPC_Data &data)
{
    Serial.println("Received Switch state");
    bool newState = data;
    Serial.print("Switch state change: ");
    Serial.println(newState);
    return RPC_Response("setLedSwitchValue", newState);
}

const std::array<RPC_Callback, 1U> callbacks = {
    RPC_Callback{"setLedSwitchValue", setLedSwitchValue}};

const Shared_Attribute_Callback attributes_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());
const Attribute_Request_Callback attribute_shared_request_callback(&processSharedAttributes, SHARED_ATTRIBUTES_LIST.cbegin(), SHARED_ATTRIBUTES_LIST.cend());

// -------------------------------------------------------
//   Envoi des données à ThingsBoard/CoreIOT (déjà existant)
// -------------------------------------------------------
void CORE_IOT_sendata(String mode, String feed, String data)
{
    if (mode == "attribute")
    {
        tb.sendAttributeData(feed.c_str(), data);
    }
    else if (mode == "telemetry")
    {
        float value = data.toFloat();
        tb.sendTelemetryData(feed.c_str(), value);
    }
}

// -------------------------------------------------------
//              Connexion au serveur CoreIOT
// -------------------------------------------------------
void CORE_IOT_reconnect()
{
    if (!tb.connected())
    {
        if (!tb.connect(CORE_IOT_SERVER.c_str(), CORE_IOT_TOKEN.c_str(), CORE_IOT_PORT.toInt()))
            return;

        tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());

        if (!tb.RPC_Subscribe(callbacks.cbegin(), callbacks.cend()))
            return;

        if (!tb.Shared_Attributes_Subscribe(attributes_callback))
            return;

        if (!tb.Shared_Attributes_Request(attribute_shared_request_callback))
            return;

        tb.sendAttributeData("localIp", WiFi.localIP().toString().c_str());

        Serial.println("[CoreIOT] Connected & subscriptions set");
    }
    else
    {
        tb.loop();
    }
}

// -------------------------------------------------------
//              TASK : Publish Temp + Humi
// -------------------------------------------------------
void coreiot_task(void *parameter)
{
    Serial.println("[CoreIOT TASK] Started.");
    startSTA();
    uint32_t lastTelemetryTime = 0;

    for (;;)
    {
        // 1) Assure la connexion MQTT/CoreIOT
        CORE_IOT_reconnect();

        // 2) Toutes les X secondes → envoi des données capteurs
        if (millis() - lastTelemetryTime >= telemetrySendInterval)
        {
            lastTelemetryTime = millis();

            float temp = glob_humidity;   // depuis temp_humi_monitor
            float hum  = glob_temperature;

            Serial.println("[CoreIOT] Sending telemetry...");
            Serial.printf("  Temp: %.2f\n", temp);
            Serial.printf("  Humi: %.2f\n", hum);

            CORE_IOT_sendata("telemetry", "temperature", String(temp));
            CORE_IOT_sendata("telemetry", "humidity", String(hum));
        }
        WiFiClient client;
        Serial.println("Testing raw TCP...");
        if (client.connect("mqtt.coreiot.io", 1883)) {
            Serial.println("TCP OK → CoreIOT reachable");
        } else {
            Serial.println("TCP FAIL → cannot reach server");
        }
        client.stop();

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
