#ifndef MEDAKA_DATA_H
#define MEDAKA_DATA_H

#include <vector>
#include "ArduinoJson.h"

struct Device {
    String name = "unknown";
    bool on = false;
    int duty = 0;
    bool automatic = false;


public:
    static StaticJsonDocument<128> toJson(const Device& d) {
        StaticJsonDocument<128> doc;
        JsonObject object = doc.to<JsonObject>();
        object["name"] = d.name;
        object["on"] = d.on;
        object["duty"] = d.duty;
        object["auto"] = d.automatic;
        return doc;
    }
};

struct Data {
    long timestamp = 0;
    double water_temperature = NAN;
    double temperature = NAN;
    double humidity = NAN;
    double atmospheric_pressure = NAN;
    double tds = NAN;
    std::vector<Device> devices;


public:
    static void dump(const Data& d) {
        Serial.print(d.timestamp);
        Serial.print(" ");
        Serial.print("water: ");
        Serial.print(d.water_temperature);
        Serial.print(", temp: ");
        Serial.print(d.temperature);
        Serial.print(", p: ");
        Serial.print(d.atmospheric_pressure);
        Serial.print(", h: ");
        Serial.print(d.humidity);
        Serial.print(", tds: ");
        Serial.print(d.tds);

        for (const auto &item: d.devices) {
            Serial.print(", ");
            Serial.print(item.name);
            Serial.print(": ");
            Serial.print(item.on);
            Serial.print(" ");
            Serial.print(item.duty);
        }

        Serial.println();
    }

    static Data merge(Data l, Data r) {
        auto next = Data();
        next.timestamp = r.timestamp ? r.timestamp : l.timestamp;
        next.water_temperature = !isnan(r.water_temperature) ? r.water_temperature : l.water_temperature;
        next.temperature = !isnan(r.temperature) ? r.temperature : l.temperature;
        next.humidity = !isnan(r.humidity) ? r.humidity : l.humidity;
        next.atmospheric_pressure = !isnan(r.atmospheric_pressure) ? r.atmospheric_pressure : l.atmospheric_pressure;
        next.tds = !isnan(r.tds) ? r.tds : l.tds;

        next.devices.insert(next.devices.end(), l.devices.begin(), l.devices.end());
        next.devices.insert(next.devices.end(), r.devices.begin(), r.devices.end());

        return next;
    }

    static StaticJsonDocument<512> toJson(const Data& d) {
        StaticJsonDocument<512> doc;
        JsonObject object = doc.to<JsonObject>();
        object["timestamp"] = d.timestamp;
        object["water_temperature"] = d.water_temperature;
        object["temperature"] = d.temperature;
        object["humidity"] = d.humidity;
        object["atmospheric_pressure"] = d.atmospheric_pressure;
        object["tds"] = d.tds;

        JsonArray devices = doc.createNestedArray("devices");
        for (const auto &item: d.devices) {
            devices.add(Device::toJson(item));
        }
        return doc;
    }
};
#endif //MEDAKA_DATA_H
