#ifndef MEDAKA_OTA_H
#define MEDAKA_OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Module.h"


class OTA : public Module {
public:
    explicit OTA() = default;

    void setup() override {
#ifdef DEVICE_NAME
        ArduinoOTA.setHostname(DEVICE_NAME);
#endif
        ArduinoOTA.onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
            } else {  // U_FS
                type = "filesystem";
            }
            Serial.println("Start updating " + type);
        });
        ArduinoOTA.onEnd([]() {
            Serial.println("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) {
                Serial.println("Auth Failed");
            } else if (error == OTA_BEGIN_ERROR) {
                Serial.println("Begin Failed");
            } else if (error == OTA_CONNECT_ERROR) {
                Serial.println("Connect Failed");
            } else if (error == OTA_RECEIVE_ERROR) {
                Serial.println("Receive Failed");
            } else if (error == OTA_END_ERROR) {
                Serial.println("End Failed");
            }
        });
        ArduinoOTA.begin();
    }

    Data loop(Data d) override {
        ArduinoOTA.handle();
        return d;
    }
};

#endif //MEDAKA_OTA_H
