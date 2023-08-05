#ifndef MEDAKA_MODULE_H
#define MEDAKA_MODULE_H

#include "ESPAsyncWebServer.h"
#include "../Data.h"

class Module {
public:
    virtual void setup() {};

    virtual void installWebAPI(AsyncWebServer*) {};

    virtual Data loop(Data d) {
        return d;
    };
};

#endif //MEDAKA_MODULE_H
