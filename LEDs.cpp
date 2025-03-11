#include "LEDs.h"
#include "Pins.h"

using namespace DEVMODE;

// Uncomment the line below to enable
// Serial.print for debugging
//
// #define DEBUG_ENABLED

void LEDs::setup() {
    this->led_pins[0] = PIN_LED_RED;
    this->led_pins[1] = PIN_LED_GREEN;
    this->led_pins[2] = PIN_LED_BLUE;
    this->led_pins[3] = PIN_LED_YELLOW;

    pinMode(this->led_pins[0], OUTPUT);
    pinMode(this->led_pins[1], OUTPUT);
    pinMode(this->led_pins[2], OUTPUT);
    pinMode(this->led_pins[3], OUTPUT);

    this->setInterval(100);
    this->randomise_led_states();
}

void LEDs::run() {
    for (uint8_t i = 0; i < 4; i++) {
        switch (led_mode[i]) {
            case 0:
#ifdef DEBUG_ENABLED
                Serial.print("Led ");
                Serial.print(i);
                Serial.println(": LOW");
#endif

                digitalWrite(led_pins[i], LOW);
                break;

            case 1:
#ifdef DEBUG_ENABLED
                Serial.print("Led ");
                Serial.print(i);
                Serial.println(": HIGH");
#endif

                digitalWrite(led_pins[i], HIGH);
                break;

            case 2:
#ifdef DEBUG_ENABLED
                Serial.print("Led ");
                Serial.print(i);
                Serial.println(": BLINK");
#endif

                led_state[i] = !led_state[i];
                digitalWrite(led_pins[i], led_state[i]);
                break;

            default:
                break;
        }
    }

    return runned();
}

void LEDs::randomise_led_states() {
// #ifdef DEBUG_ENABLED
    Serial.print("Led modes: ");
// #endif

    for (uint8_t i = 0; i < 4; i++) {
        this->led_mode[i] = random(0, 3);
// #ifdef DEBUG_ENABLED
        Serial.print(i);
        Serial.print("->");
        Serial.print(this->led_mode[i]);
        Serial.print(" ");
// #endif
    }

    Serial.println();
};


unsigned long LEDs::get_button_duration_ms() {
    const uint8_t times[] = {
        1, 0, 1, 0, 2, 1, 2, 1, 1, 3, 5, 2,
    };

    unsigned long duration = 0;
    for (uint8_t i = 0; i < 4; i++) {
        duration += times[i * 3 + led_mode[i]] * 1000;
    }

    return duration;
};
