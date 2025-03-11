#include <LiquidCrystal_I2C.h>
#include <OddOrEven.h>

#include "Frequency.h"
#include "Pins.h"

using namespace RADIO;

// Uncomment the line below to enable
// Serial.print for debugging
//
// #define DEBUG_ENABLED

static unsigned long long_difference(long a, long b);
static void offset_to_displaytext(uint16_t offset, char* displayText);

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void Frequency::setup() {
    this->setInterval(50);
    this->target = 100;

    pinMode(PIN_POTENTIOMETER, INPUT);
    this->current = analogRead(PIN_POTENTIOMETER) * 999.0 / 1024;

    lcd.init();  // initialize the lcd
    lcd.init();

    lcd.clear();

    // The backlight isn't active, but
    // let's write RADIO on the screen
    // anyway so the module is easier
    // for the user to find
    lcd.setCursor(0, 0);
    lcd.print("RADIO");

    // backlight will be activated
    // when the radio module is enabled
    // after devmode
}

void Frequency::run() {
    // this->current = analogRead(PIN_POTENTIOMETER) * 999.0 / 1024;

    // weighted average
    this->current = (0.6 * analogRead(PIN_POTENTIOMETER) * 999.0 / 1024) + (0.4 * this->current);


    unsigned long difference = long_difference(this->current, this->target);

    // if the potentiometer value is close to the target value,
    // that's considered acceptable
    if (difference < 24) {
        this->current = this->target;
        this->tuned = true;
    } else {
        this->tuned = false;
    }

    char displayText[16];

#ifdef DEBUG_ENABLED
    offset_to_displaytext(this->target, displayText);
    Serial.print("Target: ");
    Serial.println(displayText);

    offset_to_displaytext(this->current, displayText);
    Serial.print("Current: ");
    Serial.println(displayText);
#endif

    offset_to_displaytext(this->current, displayText);

    lcd.setCursor(4, 1);
    lcd.print(displayText);

    return runned();
}

void Frequency::enableLCDBacklight() {
    lcd.backlight();   
}

static unsigned long long_difference(long a, long b) {
    // if a < b, abs(a - b) will not work because of underflow rollover
    // hence this function

    return a > b ? a - b : b - a;
}

static void offset_to_displaytext(uint16_t offset, char* displayText) {
    // offset can be any number between 0 and 999
    // displayText **must** be "137.0000 MHz"

    displayText[0] = '\0';

    strncat(displayText, "137.0000 MHz", 12);

    displayText[4] = '0' + (offset / 100) % 10;
    displayText[5] = '0' + (offset / 10) % 10;
    displayText[6] = '0' + (offset / 1) % 10;
}