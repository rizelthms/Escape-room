#ifndef __RADIO_MORSE__H
#define __RADIO_MORSE__H

#include "Thread.h"

namespace RADIO {
class Morse : public Thread {
   private:
    char message[16];  // Contains english alphabet
    uint8_t message_len = 0;
    uint8_t message_index = 0;

    uint8_t morse[8];  // Contains morse character for current letter
    uint8_t morse_index = 0;

    // if set to true, no sound is played for
    // a little while to indicate next letter
    bool play_morse_space = false;
    void set_morse(char character);

   public:
    void setup();
    void run();

    void set_message(uint8_t length, char* message_);
};
}  // namespace RADIO


#endif