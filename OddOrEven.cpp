#include <TM1638plus.h>

using namespace OddOrEven;

#define CHALLENGE_ID 3

static uint8_t read_button();
static void challengeLogic();

#define PIN_LEDKEY_STB 6  
#define PIN_LEDKEY_CLK 7 ]
#define PIN_LEDKEY_DIO 8   
static TM1638plus* ledandkey = new TM1638plus(PIN_LEDKEY_STB, PIN_LEDKEY_CLK, PIN_LEDKEY_DIO);

void setup() {
    ledandkey->reset();
    ledandkey->displayBegin();
}

void loop() {
    challengeLogic();
}

static uint8_t read_button() {
    uint8_t buttonPressed = ledandkey->readButtons();

    switch (buttonPressed) {
        case 0x01:  // 0000 0001
            return 0;
        case 0x80:  // 1000 0000
            return 7;
    }

    // no button was pressed
    return 255;
}

static void challengeLogic() {
    static uint8_t nrounds = random(3, 7);
    static uint8_t sequence_length = 0;
    static uint8_t sequence_index = 0;
    static bool is_even;

    if (nrounds == 0) {
        // all rounds completed, exit the game
        return;
    }

    if (sequence_length == 0) {
        // generate a random LED count
        sequence_length = random(1, 9);
        is_even = sequence_length % 2 == 0;
    }

    static uint8_t state_machine_state = 0;
    static unsigned long last_run_at;
    static unsigned long interval = 1000;
    static uint8_t ledIndices[8]; // array to store the LED indices that have been lit

    switch (state_machine_state) {
        case 0: {
            // State 0 lights up a random number of LEDs
            // based on the generated sequence length.

            if (sequence_index == sequence_length) {
                // Finished displaying sequence,
                // turn off LEDs and wait for user
                // input.
                ledandkey->setLEDs(0);

                sequence_index = 0;
                state_machine_state = 2;
                break;
            }

            if (millis() - last_run_at < interval) {
                break;
            }

            // Choose a random LED index that hasn't been lit up yet
            uint8_t ledIndex;
            do {
                ledIndex = random(8);
            } while (ledIndices[ledIndex] == 1);
            ledIndices[ledIndex] = 1;

            ledandkey->setLED(ledIndex, 1);
            sequence_index++;
            last_run_at = millis();

            break;
        }

        case 2: {
            // State 2 is reached after state 0 finishes
            // displaying all the LEDs in the sequence.
            // State 2 captures the button presses and
            // compares them to the expected parity.

            uint8_t button = read_button();

            // No button is pressed
            if (button == 255)
                break;

            if ((is_even && button == 0) || (!is_even && button == 7)) {
                // Correct answer
                sequence_length = 0;
                nrounds--;

                if (nrounds == 0) {
                    // Challenge complete
                    ledandkey->displayText("You Pass");
                    delay(2000);
                }

                state_machine_state = 0;
                memset(ledIndices, 0, sizeof(ledIndices)); // Reset the LED indices array
            } else {
                // Incorrect answer
                ledandkey->displayText("You died");
                delay(2000);
            }

            break;
        }
    }
}
