#ifndef __OddOrEven__H
#define __OddOrEven__H

#include "Thread.h"

class OddOrEven : public Thread {
   public:
    void setup();
    void run();
};

#endif