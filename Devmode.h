#ifndef __DEVMODE__H
#define __DEVMODE__H

#include "Thread.h"

class Devmode : public Thread {
   public:
    void setup();
    void run();
};

#endif