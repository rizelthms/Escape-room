#ifndef __RADIO__H
#define __RADIO__H

#include "Thread.h"

class Radio : public Thread {
   public:
    void setup();
    void run();

    void enableLCDBacklight();
};

#endif