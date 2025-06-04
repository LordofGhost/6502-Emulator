#ifndef W65C22_H
#define W65C22_H

#include <string>

#include "../Component.h"

// Versatile Interface Adapter

class W65C22 : public Component {
   public:
    void onClockCycle(Phase phase) override;
    std::string toStringMD() const;
};

#endif  // W65C22_H
