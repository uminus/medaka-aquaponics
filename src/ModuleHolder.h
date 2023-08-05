#ifndef MEDAKA_MODULEHOLDER_H
#define MEDAKA_MODULEHOLDER_H

#include <numeric>
#include <LittleFS.h>
#include "module/Module.h"
#include "ESPAsyncWebServer.h"

#define nameof(x) #x

class ModuleHolder {
private:
    std::vector<Module *> modules_;
    AsyncWebServer *server_;
    Data lastData;

    boolean setupCompleted = false;

public:
    explicit ModuleHolder(AsyncWebServer *server) : server_(server) {}

    void install(Module *module) {
        if (setupCompleted) {
            throw std::runtime_error("The setup process is already completed.");
        }
        modules_.push_back(module);
        Serial.print("install: ");
        Serial.println(nameof(module));
    }

    void doSetup() {
        // cors
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

        // index.html
        server_->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("index.html");
            request->send(LittleFS, "/index.html", "text/html");
        });

        server_->on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
            String response;
            serializeJson(Data::toJson(lastData), response);
            request->send(200, "application/json", response);
        });


        for (const auto &item: modules_) {
            item->setup();
            item->installWebAPI(server_);
        }

        setupCompleted = true;
    }

    Data doLoop() {
        Data d = Data();
        for (const auto &module: modules_) {
            d = module->loop(d);
        }
        lastData = d;
        return d;
    }
};

#endif //MEDAKA_MODULEHOLDER_H
