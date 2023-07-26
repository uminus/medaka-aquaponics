#ifndef MEDAKA_THERMISTOR_H
#define MEDAKA_THERMISTOR_H

#include "Module.h"

class Thermistor : public Module {
private:
    unsigned int pin_;
    unsigned long prevTemp_{};
    std::deque<double> buffer_;
    const double THERM_A = 0.8880739089e-3; //  0c
    const double THERM_B = 2.514251712e-4;  // 25c
    const double THERM_C = 1.922794488e-7;  // 50c

public:
    explicit Thermistor(int pin) : pin_(pin) {}

    void setup() override {
        pinMode(pin_, INPUT);
    }

    Data loop(Data d) override {
        auto next = Data();

        // every 50ms, read analog value
        if (millis() - prevTemp_ > 50) {
            int aout = analogRead(pin_);
            double vx = 3.3 - aout * (3.3 / 4096.0);
            double rx = vx * 8800.0 / (3.3 - vx);  // 抵抗の誤差がある
            auto water_temperature = (1.0 / (THERM_A + THERM_B * log(rx) + THERM_C * pow(log(rx), 3))) - 273.15;

            if (buffer_.size() >= 10) {
                buffer_.pop_front();
            }
            buffer_.push_back(water_temperature);
            prevTemp_ = millis();
        }


        if (buffer_.size() == 10) {
            auto cloneTemps = std::vector<double>(buffer_.begin(), buffer_.end());
            std::sort(cloneTemps.begin(), cloneTemps.end());
            auto normalizedTemp = cloneTemps[cloneTemps.size() / 2];
            next.water_temperature = normalizedTemp;
        }

        return Data::merge(d, next);
    }
};

#endif //MEDAKA_THERMISTOR_H
