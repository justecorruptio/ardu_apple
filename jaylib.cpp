#include "jaylib.h"
#include "utils.h"
#include "frames.h"

void Jaylib::drawBand(uint8_t x, uint8_t y, const uint8_t * sprite, uint8_t cols) {
    uint16_t s;
    uint16_t p = (y >> 3) * WIDTH + cols + x - 1;

    for(; cols --; p --) {
        s = pgm_read_byte(sprite + cols) << (y & 0x7);

        sBuffer[p] |= s;
        sBuffer[p + WIDTH] |= s >> 8;
    }
}

void Jaylib::smallPrint(uint8_t x, uint8_t y, const uint8_t * str) {
    char c;
    for(;c = *str ++;) {
        c -= 32;
        drawBand(x, y, PRINTABLE_CHARS + 3 * c, 3);
        x += 4;
    }
}

void Jaylib::largePrint(uint8_t x, uint8_t y, const uint8_t * str, uint8_t kern) {
    char c;
    for(;c = *str ++;) {
        c &= 0x1F; // Lucky that SPACE % 32 == 0
        drawBand(x, y, PRINTABLE_CHARS_LARGE + 5 * c, 5);
        x += 5 + kern;
    }
}

void Jaylib::drawFastVLine(uint8_t x, uint8_t y, uint8_t h) {
    uint8_t i;
    for(i = h; i --;)
        drawPixel(x, y + i);
}

void Jaylib::drawFastHLine(uint8_t x, uint8_t y, uint8_t w) {
    uint8_t i;
    for(i = w; i --;)
        drawPixel(x + i, y);
}

void Jaylib::drawSquare(uint8_t x, uint8_t y, uint8_t w) {
    uint8_t i, j;
    for(i = w; i --;)
        for(j = w; j --;)
            drawPixel(x + i, y + j);
}

#define W FRAMES_W
#define H_ FRAMES_H

#define OFF_X ((128 - W * 3) / 2)
#define OFF_Y ((64 - H_ * 3) / 2)
#define pix(x, y) (!!(data[((y) * W + (x)) / 8] & ( 1 << (((y) * W + (x)) % 8))))


void Jaylib::scale3(uint8_t* data, uint8_t x, uint8_t y) {
    uint8_t A = pix(x - 1, y - 1);
    uint8_t B = pix(x + 0, y - 1);
    uint8_t C = pix(x + 1, y - 1);
    uint8_t D = pix(x - 1, y + 0);
    uint8_t E = pix(x + 0, y + 0);
    uint8_t F = pix(x + 1, y + 0);
    uint8_t G = pix(x - 1, y + 1);
    uint8_t H = pix(x + 0, y + 1);
    uint8_t I = pix(x + 1, y + 1);

    if(D==B && D!=H && B!=F ? D : E)
        drawPixel(x * 3 - 1 + OFF_X, y * 3 - 1 + OFF_Y);
    if(D==B && D!=H && B!=F && E!=C || B==F && B!=D && F!=H && E!=A ? B : E)
        drawPixel(x * 3 + 0 + OFF_X, y * 3 - 1 + OFF_Y);
    if(B==F && B!=D && F!=H ? F : E)
        drawPixel(x * 3 + 1 + OFF_X, y * 3 - 1 + OFF_Y);
    if(H==D && H!=F && D!=B && E!=A || D==B && D!=H && B!=F && E!=G ? D : E)
        drawPixel(x * 3 - 1 + OFF_X, y * 3 + 0 + OFF_Y);
    if(E)
        drawPixel(x * 3 + 0 + OFF_X, y * 3 + 0 + OFF_Y);
    if(B==F && B!=D && F!=H && E!=I || F==H && F!=B && H!=D && E!=C ? F : E)
        drawPixel(x * 3 + 1 + OFF_X, y * 3 + 0 + OFF_Y);
    if(H==D && H!=F && D!=B ? D : E)
        drawPixel(x * 3 - 1 + OFF_X, y * 3 + 1 + OFF_Y);
    if(F==H && F!=B && H!=D && E!=G || H==D && H!=F && D!=B && E!=I ? H : E)
        drawPixel(x * 3 + 0 + OFF_X, y * 3 + 1 + OFF_Y);
    if(F==H && F!=B && H!=D ? F : E)
        drawPixel(x * 3 + 1 + OFF_X, y * 3 + 1 + OFF_Y);
}
