#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <Hash.h>

#define USE_SERIAL Serial

SocketIOclient socketIO;

void setLight(uint8_t * payload) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    JsonArray array = doc.as<JsonArray>();
    JsonObject data = array[1];
    for (const auto& kv : data) {
        if (kv.key() == "b") {
            digitalWrite(5, data[kv.key()]);
        }
    }
}

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

    socketIO.begin("10.31.137.4", 7777, "/socket.io/?EIO=4");
    socketIO.onEvent(socketIOEvent);

    digitalWrite(5, 0);
}

void loop() {
    socketIO.loop();
    int a = analogRead(A0);
    if (a > 600) {
        // digitalWrite(5, HIGH);
        socketIO.sendEVENT("[\"b\",{\"sender\":\"a\",\"msg\":1}]");
    }
    else {
        // digitalWrite(5, LOW);
        socketIO.sendEVENT("[\"b\",{\"sender\":\"a\",\"msg\":0}]");
    }
    delay(10);
}