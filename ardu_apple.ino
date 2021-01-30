#include "jaylib.h"
#include "frames.h"

#include <avr/wdt.h>
ARDUBOY_NO_USB

Jaylib jay;

void setup() {
    jay.boot();
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

uint8_t PREV [W * H / 8];

uint8_t _get_nib(uint8_t *frame, uint16_t ptr) {
    return 0xf & pgm_read_byte(frame + BL + 2 + ptr / 2) >> (4 * (ptr %2));
}

int drawFrame(uint8_t *frame) {
    uint16_t len = pgm_read_word(frame + BL);

    uint8_t color = len >> 15 & 1;
    uint8_t horiz = len >> 14 & 1;
    len &= 0x3fff;

    uint16_t skip = 0;
    uint16_t ptr = 0;

    int x, y;

    for (int i = 0; i < W*H; i++) {
        if (horiz)
            x = i % W, y = i / W;
        else
            x = i / H, y = i % H;

        int pix_id = y * W + x;
        uint8_t pix_shift = 1 << (pix_id % 8);
        int block_id = y / BS * BW + x / BS;

        if( pgm_read_byte(frame + block_id / 8) & (1 << (block_id % 8)) ) {
            if (PREV[pix_id / 8] & pix_shift)
                jay.drawPixel(x * FACTOR + OFFX, y * FACTOR + OFFY);
        } else {
            while (!skip) {
                color = !color;
                if(ptr >= len) {
                    skip = 0xffff;
                    break;
                }
                skip = _get_nib(frame, ptr ++);
                if(skip == 0) {
                    uint8_t p, q = 0;
                    do {
                        p = _get_nib(frame, ptr ++);
                        skip |= (p & 0x7) << q;
                        q += 3;
                    } while(p & 0x8);
                    skip += 16;
                }
            }
            if(!color) {
                jay.drawPixel(x * FACTOR + OFFX, y * FACTOR + OFFY);
                PREV[pix_id / 8] |= pix_shift;
            } else {
                PREV[pix_id / 8] &= ~pix_shift;
            }
            skip --;
        }
    }

    return (len + 1) / 2 + BL + 2; // round up
}


uint16_t ptr;
uint8_t counter;
void loop() {

    if(!jay.nextFrame()) return;

    //jay.pollButtons();
    jay.clear();

    //if(jay.justPressed(DOWN_BUTTON))
    //    jay.exitToBootloader();
    #ifdef ARDUBOY_10
    if(~PINF & _BV(DOWN_BUTTON_BIT))
    #elif defined(AB_DEVKIT)
    if(~PINB & _BV(DOWN_BUTTON_BIT))
    #endif
        jay.exitToBootloader();

    int ret = drawFrame(FRAMES + ptr);
    counter ++;
    if (counter % 1 == 0) {
        ptr += ret;
    }

    if (ptr > FRAMES_BYTES) {
        ptr = 0;
        for(int i = 0; i < W * H / 8; i++)
            PREV[i] = 0;
    }

    jay.display();
}

// vim:syntax=c
