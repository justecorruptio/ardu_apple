#ifndef JAYLIB_H
#define JAYLIB_H

#include <Arduboy2.h>
#include <avr/power.h>

#include "jay_tones_pitches.h"
#include "jay_sprites.h"

class JayTones {
    public:
    void static on();
    void static off();

    void static init();
    void static playNote(uint8_t chan, uint16_t freq, int16_t dur);
    void static playSong(uint8_t chan, uint8_t * song);
    void static stopSong(uint8_t chan);
    void static nextNote(uint8_t chan);
};

class Jaylib: public Arduboy2Base {
    public:
    JayTones tones;

    void drawBand(uint8_t x, uint8_t y, const uint8_t * sprite, uint8_t cols);

    void smallPrint(uint8_t x, uint8_t y, const uint8_t * str);
    void largePrint(uint8_t x, uint8_t y, const uint8_t * str, uint8_t kern = 1);

    void drawFastVLine(uint8_t x, uint8_t y, uint8_t h);
    void drawFastHLine(uint8_t x, uint8_t y, uint8_t w);

    void drawSquare(uint8_t x, uint8_t y, uint8_t w);
};

#endif
