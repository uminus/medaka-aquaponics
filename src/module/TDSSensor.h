#ifndef MEDAKA_TDSSENSOR_H
#define MEDAKA_TDSSENSOR_H

#include <Arduino.h>
#include <deque>
#include "Module.h"

class TDSSensor : public Module {
private:
    unsigned long prevTdsRead_{};
    std::deque<double> buffer_;
    unsigned int pin_;

public:
    explicit TDSSensor(int pin) : pin_(pin) {}

    void setup() override {
        pinMode(pin_, INPUT);
    }

    Data loop(Data d) override {
        auto next = Data();

        if (isnan(d.water_temperature)) {
            return next;
        }

        // every 50ms, read TDS analog value
        if (millis() - prevTdsRead_ > 50) {
            auto tdsV = analogRead(pin_) * 5.0 / 4096.0;
            auto tdsCompV = tdsV / (1.0 + 0.02 * (d.water_temperature - 25.0));
            auto tds = (133.42 * pow(tdsCompV, 3) - 255.86 * pow(tdsCompV, 2) + 857.39 * tdsCompV) * 0.5;
            if (buffer_.size() >= 10) {
                buffer_.pop_front();
            }
            buffer_.push_back(tds);
            prevTdsRead_ = millis();
        }


        if (buffer_.size() == 10) {
            auto cloneTds = std::vector<double>(buffer_.begin(), buffer_.end());
            std::sort(cloneTds.begin(), cloneTds.end());
            auto normalizedTds = cloneTds[cloneTds.size() / 2];
            next.tds = normalizedTds;
        }

        return Data::merge(d, next);
    }
};

#endif //MEDAKA_TDSSENSOR_H
