#include <Arduino.h>
#include <Display.h>

Display display(0x27, 3);

void setup() {
    display.init();
    display.setBrightness(255);
    display.clear();
    display.show();
    Serial.begin(9600);
}

void loop() {
    if (Serial.available()) {
        char strInput[30];
        char command = strInput[0];
        char value = strInput[1];
        char* comma = strchr(strInput, ',');

        char strX[3];
        for (uint8_t i = 2; i < comma; i++) {
            
        }

        uint8_t x = atoi()

        if (command == 'p') {
            
        }
        else if (command == 'c') {
            display.clear();
            display.show();
        }
    }
}