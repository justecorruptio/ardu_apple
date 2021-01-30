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
    uint8_t color = pgm_read_byte(frame + BL + 1) >> 7 & 1;
    uint8_t horiz = pgm_read_byte(frame + BL + 1) >> 6 & 1;
    uint16_t len = pgm_read_byte(frame + BL);
    len |= (pgm_read_byte(frame + BL + 1) & 0x1f) << 8;
    uint16_t skip = 0;
    uint16_t ptr = 0;

    int x, y;
    uint8_t p, q;

    for (int i = 0; i < W*H; i++) {
        if (horiz) {
            x = i % W; y = i / W;
        } else {
            x = i / H; y = i % H;
        }
        int pix_id = y * W + x;
        uint8_t pix_shift = 1 << (pix_id % 8);
        int block_id = y / BS * BW + x / BS;
        if( pgm_read_byte(frame + block_id / 8) & (1 << (block_id % 8)) ) {
            if (PREV[pix_id / 8] & pix_shift) {
                jay.drawSquare(x * FACTOR + OFFX, y * FACTOR + OFFY, FACTOR);
            }
        } else {
            while (!skip) {
                color = 1 - color;
                if(ptr >= len) {
                    skip = 0xffff;
                    break;
                }
                skip = _get_nib(frame, ptr);
                ptr ++;
                if(skip == 0) {
                    q = 0;
                    while (1) {
                        p = _get_nib(frame, ptr);
                        ptr++;
                        skip |= (p & 0x7) << q;
                        if (!(p & 0x8)) {
                            break;
                        }
                        q += 3;
                    }
                    skip += 16;
                }
            }
            if(!color) {
                jay.drawSquare(x * FACTOR + OFFX, y * FACTOR + OFFY, FACTOR);
                PREV[pix_id / 8] |= pix_shift;
            } else {
                PREV[pix_id / 8] &= ~pix_shift;
            }
            skip --;
        }
    }

    len += len%2;
    return len/2 + BL + 2;
}


uint32_t ptr;
uint32_t counter;
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
