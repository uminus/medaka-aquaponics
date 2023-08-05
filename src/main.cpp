#include <Arduino.h>
#include "Data.h"
#include "ModuleHolder.h"
#include "module/DeviceSetup.h"
#include "module/TDSSensor.h"
#include "module/DCMotor.h"
#include "module/BME280.h"
#include "module/Thermistor.h"
#include "module/Timestamp.h"
#include "module/AmbientPublisher.h"
#include "module/OTA.h"

// env.build_flags からの環境変数取り込みマクロ
#define STRX(x) #x
#define STR(x) STRX(x)

#define PIN_THERMISTOR 34
#define PIN_TDS        35
#define PIN_FAN        32
#define CH_FAN         0
#define PIN_PUMP       33
#define CH_PUMP        10

AsyncWebServer server(80);
ModuleHolder holder = ModuleHolder(&server);

void setup() {
    Serial.begin(115200);

    holder.install(new DeviceSetup);
    holder.install(new OTA);
    holder.install(new Timestamp);
    holder.install(new Thermistor(PIN_THERMISTOR));
    holder.install(new BME280);
    holder.install(new TDSSensor(PIN_TDS));
    holder.install(new DCMotor("fan", CH_FAN, PIN_FAN, 70, 30000));
    holder.install(new AirCoolerFan("pump", CH_PUMP, PIN_PUMP, 90, 120));
    holder.install(new AmbientPublisher);

    holder.doSetup();
    server.begin();
}

void loop() {
    holder.doLoop();
    delay(500);
}
