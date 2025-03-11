#include "Devmode.h"
#include "LEDs.h"
#include "Pins.h"

using namespace DEVMODE;

#define CHALLENGE_ID 1
// Uncomment the line below to enable
// Serial.print for debugging
//
#define DEBUG_ENABLED

// these are defined in main.cpp
extern void explodeBomb(uint8_t challenge_ID);
extern void challengeComplete(uint8_t challenge_ID);

static unsigned long unsigned_long_difference(unsigned long a, unsigned long b);
static void challengeLogic();

static LEDs* leds = new LEDs();

void Devmode::setup() {
    leds->setup();
    pinMode(BTN_USER, INPUT_PULLUP);
    
    // TODO: REMOVE THIS
    this->setInterval(100);
};

void Devmode::run() {
    if (leds->shouldRun())
        leds->run();

    challengeLogic();

    return runned();
}

static unsigned long unsigned_long_difference(unsigned long a, unsigned long b) {
    // if a < b, abs(a - b) will not work because of underflow rollover
    // hence this function

    return a > b ? a - b : b - a;
}

static void challengeLogic() {
    static uint8_t nrounds = random(3, 6);
    static uint8_t state_machine_state = 0;

    static unsigned long button_pressed_since = 0;

    switch (state_machine_state) {
        case 0: {
            // State: Waiting for button to be
            // pressed

#ifdef DEBUG_ENABLED
            Serial.println("DEV: Waiting for button press");
#endif

            if (digitalRead(BTN_USER) == HIGH) {
                // Button not pressed
                break;
            }

            // Button has been pressed.
            state_machine_state = 1;
            button_pressed_since = millis();
            break;
        }


        case 1: {
            // State: Button is pressed, waiting
            // for it to be released
#ifdef DEBUG_ENABLED
            Serial.print("DEV: Btn Pressed for ");
            Serial.print(millis() - button_pressed_since);
            Serial.print(" ms of ");
            Serial.print(leds->get_button_duration_ms());
            Serial.println(" ms.");
#endif
            // Wait till button is released
            if (digitalRead(BTN_USER) == LOW) {
                break;
            }

            // Button has been released.
            state_machine_state = 2;
            break;
        }


        case 2: {
            // State: Button has been released,
            // was it held for the correct time?
#ifdef DEBUG_ENABLED
            Serial.print("DEV: Btn Released ");
            Serial.print(nrounds);
            Serial.print(" ");
#endif
            unsigned long button_hold_duration = millis() - button_pressed_since;

#ifdef DEBUG_ENABLED
            Serial.print("DEV: Btn held for ");
            Serial.print(button_hold_duration);
            Serial.print("  ms. error is");
            Serial.print(unsigned_long_difference(leds->get_button_duration_ms(), button_hold_duration));
            Serial.println(" ms.");
#endif
            // Was the button held for the right duration?
            // We allow a leeway of 1500 milliseconds because
            // humans can't press buttons with millisecond
            // precision.
            if (unsigned_long_difference(leds->get_button_duration_ms(), button_hold_duration) > 1500) {
                Serial.println("DEV: Error not within limits.");
                explodeBomb(CHALLENGE_ID);
                leds->enabled = false;
                state_machine_state = 3;
                break;
            }

#ifdef DEBUG_ENABLED
            Serial.println("DEV: Error within limits.");
#endif
            // If here, the button was held for the right duration.
            nrounds--;
#ifdef DEBUG_ENABLED
            Serial.println(nrounds);
#endif
            // If all rounds are done,
            if (nrounds == 0) {
#ifdef DEBUG_ENABLED
                Serial.println("DEV: Challenge complete.");
#endif
                leds->enabled = false;
                state_machine_state = 3;
                challengeComplete(CHALLENGE_ID);
                break;
            }

            leds->randomise_led_states();

            state_machine_state = 0;
            break;
        }

        case 3: {
            // State does nothing.
            // Just a state to put the challenge
            // in once all levels are done.
#ifdef DEBUG_ENABLED
            Serial.println("State 3");
#endif
            break;
        }
    }
}