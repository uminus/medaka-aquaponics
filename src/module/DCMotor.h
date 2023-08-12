#ifndef MEDAKA_DCMOTOR_H
#define MEDAKA_DCMOTOR_H

#include <utility>

#include "Module.h"

class DCMotor : public Module {
protected:
    String name_;
    unsigned int channel_;
    unsigned int pin_;
    unsigned int freq_;
    unsigned int min_;
    unsigned int duty_ = 0;
    bool is_on_ = false;

public:
    DCMotor(String name, int channel, int pin, int min, int freq)
            : name_(std::move(name)),
              channel_(channel),
              pin_(pin),
              freq_(freq),
              min_(min) {}

    void setup() override {
        ledcSetup(channel_, freq_, 8);
        ledcAttachPin(pin_, channel_);
    }

    void installWebAPI(AsyncWebServer *server) override {
        server->on((String{"/"} + name_ + "/").c_str(),
                   HTTP_GET,
                   [this](AsyncWebServerRequest *request) {
                       String response;
                       serializeJson(Device::toJson(data()), response);
                       request->send(200, "application/json", response);
                   });

        server->on((String{R"(^\/)"} + name_ + R"(\/(on|off|\d+)$)").c_str(),
                   HTTP_GET,
                   [this](AsyncWebServerRequest *request) {
                       auto op = request->pathArg(0);
                       Serial.println(op);
                       if (op == "on") {
                           turn_on(true);
                       } else if (op == "off") {
                           turn_on(false);
                       } else {
                           set_duty(op.toInt());
                       }
                       String response;
                       serializeJson(Device::toJson(data()), response);
                       request->send(200, "application/json", response);
                   });
    }

    Data loop(Data d) override {
        if (is_on_) {
            if (ledcRead(channel_) <= 0) {
                ledcWrite(channel_, 255);
                delay(100);
            }
            ledcWrite(channel_, duty_);
        } else {
            ledcWrite(channel_, 0);
        }

        auto next = Data();
        auto device = data();
        next.devices.push_back(device);

        return Data::merge(d, next);
    }

    unsigned int set_duty(int duty) {
        duty_ = min(max((int) min_, duty), 255);
        return duty_;
    }

    virtual void turn_on(bool op) {
        is_on_ = op;
        set_duty((int) duty_);
    }

    virtual Device data() {
        auto device = Device();
        device.name = name_;
        device.on = is_on_;
        device.duty = (int) duty_;
        return device;
    }

};

class AirCoolerFan : public DCMotor {
private:
    bool auto_ = false;

public:
    AirCoolerFan(String name, int channel, int pin, int min, int freq)
            : DCMotor(std::move(name), channel, pin, min, freq) {}

    void installWebAPI(AsyncWebServer *server) override {
        DCMotor::installWebAPI(server);

        server->on((String{R"(^\/)"} + name_ + R"(\/auto$)").c_str(),
                   HTTP_GET,
                   [this](AsyncWebServerRequest *request) {
                       auto_ = true;
                       String response;
                       serializeJson(Device::toJson(data()), response);
                       request->send(200, "application/json", response);
                   });
    }

    void turn_on(bool op) override {
        DCMotor::turn_on(op);
        auto_ = false;
    }

    Data loop(Data d) override {
        if (!auto_) {
            return DCMotor::loop(d);
        }

        auto next = Data();
        auto device = data();
        next.devices.push_back(device);

        if (d.water_temperature < 27.5) {
            ledcWrite(channel_, 0);
        }
        if (d.water_temperature > 28.5) {
            ledcWrite(channel_, min_);
        }
        if (d.water_temperature > 29.5) {
            ledcWrite(channel_, 255);
        }

        return Data::merge(d, next);
    }

    Device data() override {
        auto device = DCMotor::data();
        device.automatic = auto_;
        return device;
    }
};

#endif //MEDAKA_DCMOTOR_H
