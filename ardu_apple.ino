#include <Arduboy2.h>

#include "frames.h"

ARDUBOY_NO_USB

Arduboy2Base ardu;

void setup() {
    ardu.boot();
}

#define W FRAMES_W
#define H FRAMES_H

#define BS FRAMES_BS
#define BW FRAMES_BW
#define BL FRAMES_BL

#define JUMP (FRAMES_JUMP * 2)

#define FACTOR (64 / FRAMES_H)
#define OFFX ((128 - W * FACTOR) / 2)
#define OFFY ((64 - H * FACTOR) / 2)

uint8_t _get_nib(uint8_t *frame, uint16_t ptr) {
    return 0xf & pgm_read_byte(frame + BL + 2 + ptr / 2) >> (4 * (ptr %2));
}

int drawFrame(uint8_t *frame) {
    uint16_t len = pgm_read_word(frame + BL);

    uint8_t color = len >> 15 & 1;
    uint8_t horiz = len >> 14 & 1;
    uint8_t zigzag = len >> 13 & 1;
    len &= 0x1fff;

    uint16_t skip = 0;
    uint16_t ptr = 0;

    int x, y;

    for (int i = 0; i < W*H; i++) {
        if (horiz) {
            y = i / W;
            x = y % 2 && zigzag ? W - 1 - i % W : i % W;
        }
        else {
            x = i / H;
            y = x % 2 && zigzag ? H - 1 - i % H : i % H;
        }

        int block_id = y / BS * BW + x / BS;
        if( pgm_read_byte(frame + block_id / 8) & (1 << (block_id % 8)) ){
            //ardu.drawPixel(x * FACTOR + OFFX, y * FACTOR + OFFY, !ardu.getPixel(x * FACTOR + OFFX, y * FACTOR + OFFY));
            //ardu.drawPixel(x * FACTOR + OFFX, y * FACTOR + OFFY, 0);
            continue;
        }

        if(!skip) {
            color = !color;
            skip = ptr >= len ? 0xffff : _get_nib(frame, ptr ++);
        }

        if(!skip) { // only after read
            uint8_t p, q = 0;
            do {
                p = _get_nib(frame, ptr ++);
                skip |= (p & 0x7) << q;
                q += 3;
            } while(p & 0x8);
            skip += 16;
        }

        ardu.drawPixel(x * FACTOR + OFFX, y * FACTOR + OFFY, !color);
        skip --;
    }

    return (len + 1) / 2 + BL + 2; // round up
}


uint16_t ptr;
void loop() {

    //if(!ardu.nextFrame()) return;
    //ardu.delayShort(10);

    #ifdef ARDUBOY_10
    if(~PINF & _BV(DOWN_BUTTON_BIT))
    #elif defined(AB_DEVKIT)
    if(~PINB & _BV(DOWN_BUTTON_BIT))
    #endif
        ardu.exitToBootloader();

    uint8_t ret = drawFrame(FRAMES + ptr);
    //if(~PINF & _BV(RIGHT_BUTTON_BIT))
        ptr += ret;

    if (ptr >= FRAMES_BYTES)
        ptr = 0;

    ardu.display(!ptr);
}

// avr-objdump -C -S -j.text

// vim:syntax=c
