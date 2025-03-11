#ifndef __DEVMODE_LEDS__H
#define __DEVMODE_LEDS__H

#include "Thread.h"

namespace DEVMODE {
class LEDs : public Thread {
   private:
    uint8_t led_pins[4];
    uint8_t led_mode[4] = {0, 0, 0, 0};
    uint8_t led_state[4] = {0, 0, 0, 0};

   public:
    void setup();
    void run();

    void randomise_led_states();
    unsigned long get_button_duration_ms();
};
}  // namespace DEVMODE


#endif