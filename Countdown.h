#ifndef __COUNTDOWN__H
#define __COUNTDOWN__H

#include "Thread.h"

class Countdown : public Thread {
   public:
    void setup();
    void run();
};

#endif