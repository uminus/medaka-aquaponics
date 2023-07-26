#ifndef MEDAKA_TIMESTAMP_H
#define MEDAKA_TIMESTAMP_H


#include "Module.h"

class Timestamp : public Module {
private:

public:
    Timestamp() = default;

    Data loop(Data d) override {
        auto next = Data();
        next.timestamp = time(nullptr);
        return Data::merge(d, next);
    }
};

#endif //MEDAKA_TIMESTAMP_H
