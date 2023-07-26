#ifndef MEDAKA_AMBIENTPUBLISHER_H
#define MEDAKA_AMBIENTPUBLISHER_H

#include "Ambient.h"
#include "Module.h"

#ifdef AMBIENT_CLIENT_ID
#pragma message STR(AMBIENT_CLIENT_ID)
#endif

#ifdef AMBIENT_WRITE_KEY
#pragma message STR(AMBIENT_WRITE_KEY)
#endif

class AmbientPublisher : public Module {
private:
    Ambient ambient;
    WiFiClient client;
    long next = 0;

public:
    AmbientPublisher() = default;

    void setup() override {
        ambient.begin(AMBIENT_CLIENT_ID, AMBIENT_WRITE_KEY, &client);
    }

    Data loop(Data d) override {
        auto t = d.timestamp;

        if (t > 60 && next < t) {
            Serial.print("[ambient] ");
            ambient.set(1, String(d.water_temperature).c_str());
            ambient.set(2, String(d.temperature).c_str());
            ambient.set(3, String(d.humidity).c_str());
            ambient.set(4, String(d.atmospheric_pressure).c_str());
            ambient.set(5, String(d.tds).c_str());

            ambient.send();
            next = t - t % 60 + 59;
            Data::dump(d);
        }

        return d;
    }
};

#endif //MEDAKA_AMBIENTPUBLISHER_H
