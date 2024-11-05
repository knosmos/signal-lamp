#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <Hash.h>

#define USE_SERIAL Serial1

SocketIOclient socketIO;

// Parameters
char name[64] = "b";        // who am I?
char endpoint[64] = "a";    // who am I listening to?

// State variables
int state = 0;
int prev_state = -1;

// Decode socketio messages and switch light accordingly
void setLight(uint8_t * payload) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    JsonArray array = doc.as<JsonArray>();
    JsonObject data = array[1];
    for (const auto& kv : data) {
        if (kv.key() == endpoint) {
            digitalWrite(5, data[kv.key()]);
        }
    }
}

// Socketio handler
void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            USE_SERIAL.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
            USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);
            // join default namespace (no auto join in Socket.IO V3)
            socketIO.send(sIOtype_CONNECT, "/");
            break;
        case sIOtype_EVENT:
            USE_SERIAL.printf("[IOc] get event: %s\n", payload);
            setLight(payload);
            break;
        case sIOtype_ACK:
            USE_SERIAL.printf("[IOc] get ack: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_ERROR:
            USE_SERIAL.printf("[IOc] get error: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_EVENT:
            USE_SERIAL.printf("[IOc] get binary: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_ACK:
            USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
            hexdump(payload, length);
            break;
    }
}

void setup() {
    // Serial/Hardware Setup
    Serial.begin(9600);
    pinMode(5, OUTPUT);

    // WiFi Setup
    WiFi.begin("MIT", "Juj6teG$Mk");

    Serial.print("Connecting");
    int blinkState = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        blinkState = (blinkState + 1) % 2;
        digitalWrite(5, blinkState);
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    // Socketio server connection
    socketIO.begin("10.31.137.4", 7777, "/socket.io/?EIO=4");
    socketIO.onEvent(socketIOEvent);

    digitalWrite(5, 0);
}

void loop() {
    socketIO.loop();
    // Read capacitive touch
    int a = analogRead(A0);
    char result[128];
    if (a > 600) {
        state = 1;
    }
    else {
        state = 0; 
    }
    // Broadcast
    if (state != prev_state) {
        snprintf(result, 128, "[\"b\",{\"sender\":\"%s\",\"msg\":%d}]", name, state);
        socketIO.sendEVENT(result);
    }
    prev_state = state;
    delay(10);
}