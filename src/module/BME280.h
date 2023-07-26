#ifndef MEDAKA_BME280_H
#define MEDAKA_BME280_H

#include "Module.h"
#include "Adafruit_BME280.h"

class BME280 : public Module {
private:
    Adafruit_BME280 bme; // I2C

public:
    BME280() = default;

    void setup() override {
        unsigned status = bme.begin(0x76);
        if (!status) {
            Serial.println("fail: BME280");
            return;
        }
    }

    Data loop(Data d) override {
        auto next = Data();

        next.temperature = bme.readTemperature();
        next.humidity = bme.readHumidity();
        next.atmospheric_pressure = bme.readPressure() / 100.0;

        return Data::merge(d, next);
    }
};

#endif //MEDAKA_BME280_H
