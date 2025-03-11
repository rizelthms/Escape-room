#include "Radio.h"
#include "Frequency.h"
#include "Morse.h"

using namespace RADIO;

#define CHALLENGE_ID 2
// Uncomment the line below to enable
// Serial.print for debugging
//
// #define DEBUG_ENABLED

// these are defined in main.cpp
extern void explodeBomb(uint8_t challenge_ID);
extern void challengeComplete(uint8_t challenge_ID);
extern char password[12];

static unsigned long long_difference(long a, long b);
static void challengeLogic();

static Morse* morse = new Morse();
static Frequency* frequency = new Frequency();


void Radio::setup() {
    frequency->setup();
    morse->setup();

    
}

void Radio::run() {
    if (frequency->shouldRun())
        frequency->run();

    if (morse->shouldRun())
        morse->run();

    challengeLogic();

    return runned();
}

void Radio::enableLCDBacklight() {
    // LCD is actually controlled by the
    // Frequency controller thread.

    frequency->enableLCDBacklight();
}

static unsigned long long_difference(long a, long b) {
    // if a < b, abs(a - b) will not work because of underflow rollover
    // hence this function

    return a > b ? a - b : b - a;
}

static void challengeLogic() {
   static uint8_t nrounds = random(4, 8);
    // Note that these are static variables.
    // Their values persist across calls.

    static uint16_t prev_freq_target = 0;
    static uint8_t state_machine_state = 0;

    static unsigned long tuned_timer = millis();

#ifdef DEBUG_ENABLED
    Serial.print(frequency->nrounds);
    Serial.print(" ");
    Serial.print(state_machine_state);
    Serial.print(" ");
    Serial.println(tuned_count);
#endif

    switch (state_machine_state) {
        case 0: {
            // in state 0, morse is only enabled
            // when frequency is same as target
            if (!frequency->tuned) {
                tuned_timer = millis();
                morse->enabled = false;
                break;
            }

            if (frequency->nrounds == 0) {
                state_machine_state = 2;
                break;
            }

            // The user could just rotate the potentiometer
            // from one end to the other and that could count
            // as finishing the round, to prevent that, we keep
            // track of how long the user stays in the correct
            // position, and only mark the round done if it's more
            // that 2000 milli seconds
            if (millis() - tuned_timer < 2000)
                break;
            tuned_timer = millis();

            --(frequency->nrounds);

            // save the current frequency for future use
            prev_freq_target = frequency->target;

            // once user tunes to the right frequency,
            // we select a new target frequency
            uint16_t next_freq = random(0, 900);

            // And make Morse play the difference in morse
            int16_t difference = next_freq - frequency->target;

            char buffer[8];
            sprintf(buffer, "%d", difference);

            morse->set_message(8, buffer);
            morse->enabled = true;

            frequency->target = next_freq;

            // then we set the logic to next state
            state_machine_state = 1;

            break;
        }

        case 1: {
            // the frequency target is no longer
            // equal to the current frequency, so
            // state(0) would have stopped playing Morse,
            // but in this state, we keep playing till the
            // user turns the potentiometer

            unsigned long difference = long_difference(frequency->current, prev_freq_target);

            // if the potentiometer value is close to the target value,
            // that's considered acceptable
            if (difference > 100) {
                morse->enabled = false;
                state_machine_state = 0;
                break;
            }

            morse->enabled = true;
            break;
        }

        case 2: {
            // case 2 means the challenge has been solved
            // we make Morse play the password

            // Note that the state_machine_state variable isn't
            // being modified in this state. Once this state
            // is reached, the state_machine always stays in
            // this state

            morse->set_message(8, password);
            morse->enabled = true;
            frequency->showpass = true;

            challengeComplete(CHALLENGE_ID);

            break;
        }

        default:
            break;
    }
}