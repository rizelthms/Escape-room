#include "Countdown.h"
#include <TM1638plus.h>
#include "Pins.h"

// using namespace COUNTDOWN;

#define CHALLENGE_ID 4
#define MAXLIFESPAN_MS 900000  // 15 * 60 * 1000 - 15 minutes
// Uncomment the line below to enable
// Serial.print for debugging
//
#define DEBUG_ENABLED


// these are defined in main.cpp
extern void explodeBomb(uint8_t challenge_ID);
extern void challengeComplete(uint8_t challenge_ID);

static uint8_t read_button();
static bool verifySequence(uint8_t actual[8], uint8_t input[8], uint8_t length);
static void createSequence(uint8_t sequence[8], uint8_t length);
static void challengeLogic();

static TM1638plus* ledandkey = new TM1638plus(PIN_LEDKEY_STB, PIN_LEDKEY_CLK, PIN_LEDKEY_DIO);

void Countdown::setup() {
    ledandkey->reset();
    ledandkey->displayBegin();
}

void Countdown::run() {
    challengeLogic();

    return runned();
}

static uint8_t read_button() {
    uint8_t buttonPressed = ledandkey->readButtons();

    // Convert from bit mask to index
    switch (buttonPressed) {
        case 0x01:  // 0000 0001
            return 0;
        case 0x02:  // 0000 0010
            return 1;
        case 0x04:  // 0000 0100
            return 2;
        case 0x08:  // 0000 1000
            return 3;
        case 0x10:  // 0001 0000
            return 4;
        case 0x20:  // 0010 0000
            return 5;
        case 0x40:  // 0100 0000
            return 6;
        case 0x80:  // 1000 0000
            return 7;
    }

    // no button was pressed
    return 255;
}

static bool verifySequence(uint8_t actual[8], uint8_t input[8], uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {

#ifdef DEBUG_ENABLED
        Serial.print("Expected button: ");
        Serial.println(actual[i]);
        Serial.print("Button pressed: ");
        Serial.println(input[i]);
#endif

        if (actual[i] != input[i])
            return false;
    }

    return true;
} 

static void createSequence(uint8_t sequence[8], uint8_t length) {
    
    for (uint8_t i = 0; i < length; i++) {
        sequence[i] = random(0, 8);
        // repeat if a number is repeated
        if(i > 0 && sequence[i] == sequence[i-1]) i--;
    }

    Serial.println("Finished creating sequence");
}

static void challengeLogic() {
    if (millis() > MAXLIFESPAN_MS) {
        // if it's been more than 15 minutes since
        // Arduino started, explode the bomb
        explodeBomb(CHALLENGE_ID);
    }

    // Display countdown on LED&KEY
    unsigned long seconds_remaining = ((MAXLIFESPAN_MS - millis()) / 1000) % 60;
    unsigned long minutes_remaining = ((MAXLIFESPAN_MS - millis()) / 60000) % 60;

    char ledkey_displaytext[12];

// #ifdef DEBUG_ENABLED
//     sprintf(ledkey_displaytext, "%4lu%4lu", minutes_remaining, seconds_remaining);
//     Serial.print("Time remaining: ");
//     Serial.println(ledkey_displaytext);
// #endif

    sprintf(ledkey_displaytext, "%4lu%4lu", minutes_remaining, seconds_remaining);
    ledandkey->displayText(ledkey_displaytext);

    // ledandkey->displayASCII(1, '3');
    // ledandkey->displayASCII(2, '4');
    // ledandkey->displayASCII(3, '1');


    // Sequence display and reading
    // static uint8_t nrounds = random(3, 5);
    static uint8_t nrounds = 2;

    static uint8_t sequence[8];
    static uint8_t sequence_length = 0;

    static uint8_t input[8];
    static uint8_t input_length = 0;
    static uint8_t old_seq_last_inp = 255;


    if (sequence_length == 0) {
        createSequence(sequence, 9 - nrounds);
        sequence_length = 9 - nrounds;
    }

    static uint8_t state_machine_state = 0;
    static uint8_t led_state = 0;
    #ifdef DEBUG_ENABLED
    // Serial.print("current state: ");
    // Serial.println(state_machine_state);
    // Serial.print("sequence length");
    // Serial.println(sequence_length);
#endif
    switch (state_machine_state) {
        case 0: {
            // State 0 lights up an LED, keeps
            // it lit for 1 second, and then jumps
            // to state 1, which turns off all
            // LEDs for half a second.

            static uint8_t sequence_index = 0;
            static unsigned long last_run_at;
            static unsigned long interval = 1000;

            if (sequence_index == sequence_length) {
                // Finished displaying sequence,
                // turn off LEDs and wait for user
                // input.
                ledandkey->setLEDs(0);

                sequence_index = 0;
                state_machine_state = 2;
                break;
            }
            
            if(led_state==0){
                Serial.print("LED ");
                Serial.println(sequence[sequence_index]);
                ledandkey->setLED(sequence[sequence_index], 1);
                led_state=1;
                last_run_at=millis();
                break;
            }

            if (millis() - last_run_at < interval) {
                break;
            }
            sequence_index++;
            state_machine_state = 1;
            // Serial.print("sequence index");
            // Serial.println(sequence_index);
            break;
        }
         case 1: {
            
            // This is a "waiting state" for state 0
            // After lighting up the led in the sequence
            // for a while, turn all leds of for a few milliseconds
            // and wait.
            // This wait makes the jump from one LED to the next
            // much less jarring.
            // Once we finish waiting, we jump back to the
            // previous state to display the next LED
            // in the sequence.
            static unsigned long last_run_at;
            if(led_state==1){
                led_state=0;
                last_run_at = millis();
                ledandkey->setLEDs(0);
            }
            
            if (millis() - last_run_at < 500) {
                break;
            }

            state_machine_state = 0;
            break;
        }

        case 2: {
            // State 2 is reached after state 0
            // finishes displaying all the numbers
            // in the sequence. State 2 captures
            // the button presses and compares them
            // to the sequence at each step.

            // ideally, if there's no user input
            // in state 2, we should go back to
            // state 0 to display the sequence again.
            // But that isn't implmented yet.
            uint8_t button = read_button();

            // No button is pressed
            if (button == 255)
                break;
            if(button == old_seq_last_inp)
                break;
            old_seq_last_inp=button;
// Button is same as last.
            // Numbers don't repeat in our sequence,
            // so this means the button is
            // still held down.
            if (input_length >= 1) {
                if (button == input[input_length - 1])
                    break;
            }

            input[input_length] = button;
            input_length++;
#ifdef DEBUG_ENABLED
    Serial.print("input_length: ");
    Serial.println(input_length);
    Serial.print("sequence entry: ");
    Serial.println(sequence[input_length-1]);
    Serial.print("input given: ");
    Serial.println(input[input_length-1]);
#endif
            if (!verifySequence(sequence, input, input_length)) {
                ledandkey->displayText("you died");
                explodeBomb(CHALLENGE_ID);
                break;
            }

            if (input_length < sequence_length) {
                // There's still more buttons
                // left to be pressed.
                break;
            }

            // Complete sequence has been entered
            // correctly.
            nrounds--;
            if (nrounds == 0) {
                challengeComplete(CHALLENGE_ID);
                break;
            }

            // Round complete.
            
            input_length = 0;
            sequence_length = 0;

            state_machine_state = 0;
            break;
        }
    }
}