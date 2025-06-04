#ifndef COMPONENT_H
#define COMPONENT_H

#include "./clock/CrystalOscillator.h"

class Component {
    public:
     virtual void onClockCycle(Phase phase) = 0;
     virtual ~Component() = default;
};

#endif //COMPONENT_H
