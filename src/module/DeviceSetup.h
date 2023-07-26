#ifndef MEDAKA_WIFISETUP_H
#define MEDAKA_WIFISETUP_H

#include <WiFi.h>
#include <LittleFS.h>
#include "Module.h"


#define JST (3600 * 9)

#ifdef WIFI_SSID
#pragma message STR(WIFI_SSID)
#endif

#ifdef WIFI_PASSWORD
#pragma message STR(WIFI_PASSWORD)
#endif

class DeviceSetup : public Module {
private:

public:
    DeviceSetup() = default;

    void setup() override {
        if (!LittleFS.begin(true)) {
            Serial.println("An Error has occurred while mounting SPIFFS");
            return;
        }
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.print("Connecting to WiFi");
        while (WiFiClass::status() != WL_CONNECTED) {
            Serial.print(".");
            delay(500);
        }
        Serial.println();
        configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
        Serial.println(WiFi.localIP());
    }
};

#endif //MEDAKA_WIFISETUP_H
