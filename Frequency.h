#ifndef __RADIO_FREQUENCY__H
#define __RADIO_FREQUENCY__H

#include "Thread.h"

namespace RADIO {
class Frequency : public Thread {
   public:
    uint16_t current;
    uint16_t target;

    // if true when current == target
    bool tuned = false;

    void setup();
    void run();

    void enableLCDBacklight();
};
}  // namespace RADIO


#endif