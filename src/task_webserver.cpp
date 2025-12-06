#include "task_webserver.h"
#include <Adafruit_NeoPixel.h>


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool webserver_isrunning = false;
bool led1State = false;
bool led2State = false;

#define LED1_PIN 2
#define LED2_PIN 4

void Webserver_sendata(String data)
{
    if (ws.count() > 0) {
        ws.textAll(data);
        Serial.println("📤 Sent WebSocket data: " + data);
    } else {
        Serial.println("⚠️ No WebSocket client connected!");
    }
}

void handleWebSocketMessage2(String message)
{
    Serial.println("WS msg received: " + message);
    
    if (message == "toggle_led1") {
        Adafruit_NeoPixel strip(1, 45, NEO_GRB + NEO_KHZ800);
        strip.begin();
        strip.clear();                                
        led1State = !led1State;
        if(led1State){
            strip.setPixelColor(0, strip.Color(255, 0, 0)); // Set pixel 0 to red
            strip.show(); // Update the strip
        }
        else{
            strip.setPixelColor(0, strip.Color(0, 0, 0)); // Turn pixel 0 off
            strip.show(); // Update the strip
        }        
        Webserver_sendata(String("{\"led1\":") + (led1State ? "1" : "0") + "}");
        }

    if (message == "toggle_led2") {
        led2State = !led2State;
        pinMode(48,0x03);
        if(led2State){
            digitalWrite(48, 0x1);
        }
        else{
            digitalWrite(48, 0x0);
        }
        Webserver_sendata(String("{\"led2\":") + (led2State ? "1" : "0") + "}");
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT) {
        Serial.printf("Client #%u connected\n", client->id());
        client->text("{\"status\":\"connected\"}");
    }
    else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("Client #%u disconnected\n", client->id());
    }
    else if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->opcode == WS_TEXT) {
            String msg = String((char*)data).substring(0, len);
            handleWebSocketMessage2(msg);
        }
    }
}

void connnectWSV()
{
    // --- ACCESS POINT MODE ---
    WiFi.mode(WIFI_AP);
    WiFi.softAP("MyESP-AccessPoint", "12345678");  
    Serial.println("AP IP address: " + WiFi.softAPIP().toString());


    ws.onEvent(onEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/script.js", "application/javascript");
    });
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/styles.css", "text/css");
    });

    server.begin();
    ElegantOTA.begin(&server);
    webserver_isrunning = true;
}

void Webserver_stop()
{
    ws.closeAll();
    server.end();
    webserver_isrunning = false;
}
void task_webserver(void* parameter){
    while(true){
        if (!LittleFS.begin()) {
            Serial.println("LittleFS mount failed");
        } 
        Webserver_reconnect();
    }
}
void Webserver_reconnect()
{
    if (!webserver_isrunning) {
        connnectWSV();
    }
    ElegantOTA.loop();
}
