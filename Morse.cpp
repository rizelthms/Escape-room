#include "Morse.h"
#include "Pins.h"

using namespace RADIO;

#define MORSE_DOT (uint8_t)10
#define MORSE_DASH (uint8_t)20
#define MORSE_END (uint8_t)0

#define MORSE_DELAY_TONEEND 300 // delay after playing dot or dash
#define MORSE_DELAY_CHAREND 900 // delay after each letter
#define MORSE_DELAY_WORDEND 3000// delay after word (entire message in this case)

#define MORSE_DURATION_DOT 300
#define MORSE_DURATION_DASH 900


// Dot and Dash are played with different frequencies
// to make distinguishing them easier.
// Dot uses a higher frequency because Dots
// are shorter and Humans associate smaller
// entities with high frequency sounds
#define MORSE_FREQUENCY_DOT 4800
#define MORSE_FREQUENCY_DASH 2048

// Uncomment the line below to enable 
// Serial.print for debugging
//
// #define DEBUG_ENABLED

void Morse::setup() {
    this->setInterval(10);
}

void Morse::run() {
    if (this->message_len == 0)
        return runned();

    if (this->play_morse_space) {
        noTone(PIN_BUZZER);
        this->play_morse_space = false;

        this->_cached_next_run = millis() + 300;
        return;
    }

    uint8_t morsetone = this->morse[this->morse_index];

    #ifdef DEBUG_ENABLED
        Serial.print("MT: ");
        Serial.print(morsetone);
        Serial.print(" ; ");
        Serial.println(this->message);
    #endif
    
    if (morsetone == MORSE_END) {
        this->message_index += 1;

        if (this->message_index == this->message_len) {
            this->message_index = 0;
            this->morse_index = 0;

            this->set_morse(this->message[this->message_index]);

            // wait for 3 seconds before running again
            this->_cached_next_run = millis() + MORSE_DELAY_WORDEND;
            return;  // don't run runned() here
        }

        this->set_morse(this->message[this->message_index]);
        this->morse_index = 0;

        // run again immediately
        this->_cached_next_run = millis() + MORSE_DELAY_CHAREND;
        return;
    }

    if (morsetone == MORSE_DOT) {
        this->morse_index += 1;
        this->play_morse_space = true;

        tone(PIN_BUZZER, MORSE_FREQUENCY_DOT, MORSE_DURATION_DOT);
        this->_cached_next_run = millis() + MORSE_DELAY_TONEEND;
        return;
    }

    if (morsetone == MORSE_DASH) {
        this->morse_index += 1;
        this->play_morse_space = true;

        tone(PIN_BUZZER, MORSE_FREQUENCY_DASH, MORSE_DURATION_DASH);
        this->_cached_next_run = millis() + MORSE_DELAY_TONEEND;
        return;
    }

    return runned();
}

void Morse::set_message(uint8_t length, char* message_) {
    this->message[0] = '\0';
    this->message_len = length;
    this->message_index = 0;
    this->morse_index = 0;

    strncat(this->message, message_, length);
}

void Morse::set_morse(char character) {

    this->morse[0] = MORSE_END;
    this->morse[1] = MORSE_END;
    this->morse[2] = MORSE_END;
    this->morse[3] = MORSE_END;
    this->morse[4] = MORSE_END;
    this->morse[5] = MORSE_END;
    this->morse[6] = MORSE_END;
    this->morse[7] = MORSE_END;

    #ifdef DEBUG_ENABLED
        Serial.print("MC: ");
        Serial.println(character);
    #endif

    this->morse_index = 0;

    switch (character) {

        case 'a':  // .-
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DASH;
            break;
        case 'b':  // -...
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            break;
        case 'c':  // -.-.
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DOT;
            break;
        case 'd':  // -..
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            break;
        case 'e':  // .
            this->morse[0] = MORSE_DOT;
            break;
        case 'f':  // ..-.
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DOT;
            break;
        case 'g':  // --.
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DOT;
            break;
        case 'h':  // ....
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            break;
        case 'i':  // ..
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            break;
        case 'j':  // .---
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DASH;
            break;
        case 'k':  // -.-
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DASH;
            break;
        case 'l':  // .-..
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            break;
        case 'm':  // --
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            break;
        case 'n':  // -.
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            break;
        case 'o':  // ---
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            break;
        case 'p':  // .--.
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DOT;
            break;
        case 'q':  // --.-
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DASH;
            break;
        case 'r':  // .-.
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DOT;
            break;
        case 's':  // ...
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            break;
        case 't':  // -
            this->morse[0] = MORSE_DASH;
            break;
        case 'u':  // ..-
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DASH;
            break;
        case 'v':  // ...-
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DASH;
            break;
        case 'w':  // .--
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            break;
        case 'x':  // -..-
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DASH;
            break;
        case 'y':  // -.--
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DASH;
            break;
        case 'z':  // --..
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            break;
        case '0':  // -----
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DASH;
            this->morse[4] = MORSE_DASH;
            break;
        case '1':  // .----
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DASH;
            this->morse[4] = MORSE_DASH;
            break;
        case '2':  // ..---
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DASH;
            this->morse[4] = MORSE_DASH;
            break;
        case '3':  // ...--
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DASH;
            this->morse[4] = MORSE_DASH;
            break;
        case '4':  // ....-
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            this->morse[4] = MORSE_DASH;
            break;
        case '5':  // .....
            this->morse[0] = MORSE_DOT;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            this->morse[4] = MORSE_DOT;
            break;
        case '6':  // -....
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            this->morse[4] = MORSE_DOT;
            break;
        case '7':  // --...
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            this->morse[4] = MORSE_DOT;
            break;
        case '8':  // ---..
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DOT;
            this->morse[4] = MORSE_DOT;
            break;
        case '9':  // ----.
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DASH;
            this->morse[2] = MORSE_DASH;
            this->morse[3] = MORSE_DASH;
            this->morse[4] = MORSE_DOT;
            break;
        case '-':
            this->morse[0] = MORSE_DASH;
            this->morse[1] = MORSE_DOT;
            this->morse[2] = MORSE_DOT;
            this->morse[3] = MORSE_DOT;
            this->morse[4] = MORSE_DOT;
            this->morse[5] = MORSE_DASH;
        default:
            // unrecognized character
            // handle the error or do nothing
            break;
    }
}
