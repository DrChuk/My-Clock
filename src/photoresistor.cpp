#include <Arduino.h>

#define PHOTO_PIN A0

int getBright() {
    return analogRead(PHOTO_PIN);
}