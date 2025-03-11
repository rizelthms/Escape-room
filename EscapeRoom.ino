#include <Arduino.h>

#include "Countdown.h"
#include "Devmode.h"
#include "Radio.h"
#include "OddOrEven.h"

// Uncomment the line below to enable
// Serial.print for debugging
//
#define DEBUG_ENABLED

void explodeBomb();
void defuseBomb();
void challengeComplete();
void generatePassword(char password[12]);
void serialInputHandler();

Devmode* devmode = new Devmode();
Radio* radio = new Radio();
Countdown* countdown = new Countdown();
OddOrEven* oddoreven = new OddOrEven();

char password[12];

void setup() {
    randomSeed(analogRead(2));

    while (!Serial)
        ;  // wait for Serial

    Serial.begin(115200);
    Serial.println("Startup");

    // fill with random digits
    // and lowercase letters
    generatePassword(password);

    devmode->setup();
    radio->setup();
    countdown->setup();
    oddoreven->setup();

    devmode->enabled = true;
    radio->enabled = false;
    oddoreven->enabled = false;
    countdown->enabled = true;
}

void loop() {
    // countdown should only be run after
    // oddOrEven has succesfully been completed
    // because both of them use the led&key

    if (Serial.available())
        serialInputHandler();

    if (devmode->shouldRun())
        devmode->run();

    if (radio->shouldRun())
        radio->run();

    if (oddoreven->shouldRun())
        oddoreven->run();

    if (countdown->shouldRun())
        countdown->run();
}

void generatePassword(char password[12]) {
    // fills the password buffer with random
    // lowercase letters and digits

    for (int i = 0; i < 8; i++) {
        int random_num = random(0, 36);  // generate a random number between 0 and 35
        if (random_num < 26) {
            // random letter
            password[i] = 'a' + random_num;  // use the random number to select a random lowercase letter
        } else {
            // random digit
            password[i] = '0' + random_num - 26;  // use the random number to select a random digit
        }
    }

    password[8] = '\0';  // terminate the string with a null character
}

void explodeBomb(uint8_t challenge_ID) {
    // call this when the
    // user fails at a challenge

    Serial.print("Bomb exploded. ");
    switch (challenge_ID) {
        case 1:
            Serial.print("DEVMODE");
            break;

        case 2:
            Serial.print("RADIO");
            break;

        case 3:
            Serial.print("OddOrEven");
            break;

        case 4:
            Serial.print("COUNTDOWN");
            break;
        default:
            break;
    }

    Serial.println(" challenge failed.");
    while (1)
        ;
}

void defuseBomb() {
    Serial.println("Success. Bomb defused.");
    while (1)
        ;
}

void challengeComplete(uint8_t challenge_ID) {
    // Challenge completion logic

    switch (challenge_ID) {
        case 1:
            // DEVMODE challenge complete
            // we can deactivate devmode thread
            // Let's also enable the radio
            // module by activating it's thread

#ifdef DEBUG_ENABLED
            Serial.println("Devmode challenge complete.");
#endif

            devmode->enabled = false;
            oddoreven->enabled = true;

            // calling radio->setup will activate
            // the LEDs backlight
            radio->enableLCDBacklight();
            radio->enabled = true;
            break;

        case 2:
            // Frequency challenge complete

            // DO NOT disable the radio challenge here.
            // The radio challenge thread will keep playing
            // morse code of the password.

            // Odd or even module has already been enabled
            // by the DEVMODE challenge, so
            // there isn't much to do here.

#ifdef DEBUG_ENABLED
            Serial.println("Frequency challenge complete.");
#endif

            break;

        case 3:
            // OddOrEven challenge complete

#ifdef DEBUG_ENABLED
            Serial.println("OddOrEven challenge complete.");
#endif

            oddoreven->enabled = false;

        case 4:
            // COUNTDOWN challenge complete

            // this is the last challenge and
            // this means that the bomb has been
            // defused. Let's deactivate the countdown
            // timer thread, this stops the countdown

#ifdef DEBUG_ENABLED
            Serial.println("Countdown challenge complete.");
#endif

            countdown->enabled = false;

        default:
            break;
    }
}

void serialInputHandler() {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "skip 1") {
        Serial.println("Challenge DEVMODE skipped");
        challengeComplete(1);
        oddoreven->enabled = false;
    } else if (input == "skip 2") {
        Serial.println("Challenge Frequency skipped");
        challengeComplete(1);
        challengeComplete(2);
        Serial.print("The password is: ");
        Serial.println(password);
    } else if (input == "skip 3") {
        Serial.println("Challenge OddOrEven skipped");
        challengeComplete(1);
        challengeComplete(2);
        challengeComplete(3);
    } else if (input == "skip 4") {
        Serial.println("Challenge COUNTDOWN skipped");
        challengeComplete(1);
        challengeComplete(2);
        challengeComplete(3);
        challengeComplete(4);
    }
}