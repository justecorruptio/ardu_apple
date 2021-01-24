#include "jaylib.h"
#include "utils.h"
#include "frames.h"

Jaylib jay;

void setup() {
    jay.boot();
}

#define W FRAMES_W
#define H FRAMES_H

#define BS FRAMES_BS
#define BW FRAMES_BW
#define BL FRAMES_BL

#define FACTOR (64 / FRAMES_H)
#define OFFX ((128 - W * FACTOR) / 2)
#define OFFY ((64 - H * FACTOR) / 2)

uint8_t PREV [W * H / 8];

int drawFrame(uint8_t *frame) {
    uint8_t color = pgm_read_byte(frame + BL) >> 7 & 1;
    uint8_t len = pgm_read_byte(frame + BL) & 0x7F;
    int16_t skip = 0;
    uint8_t ptr = 0;

    for( int y = 0; y < H; y ++) {
        for( int x = 0; x < W; x ++) {
            int pix_id = y * W + x;
            int block_id = y / BS * BW + x / BS;
            if( pgm_read_byte(frame + block_id / 8) & (1 << (block_id % 8)) ) {
                if ((PREV[pix_id / 8] & (1 << (pix_id % 8)))) {
                    jay.drawSquare(x * FACTOR + OFFX, y * FACTOR + OFFY, FACTOR);
                }
            } else {
                while (!skip) {
                    color = 1 - color;
                    if(ptr >= len) {
                        skip = 0xffff;
                        break;
                    }
                    skip = 0xf & pgm_read_byte(frame + BL + 1 + ptr / 2) >> (4 * (ptr %2));
                    ptr ++;
                }
                if(!color) {
                    jay.drawSquare(x * FACTOR + OFFX, y * FACTOR + OFFY, FACTOR);
                    PREV[pix_id / 8] |= (1 << (pix_id % 8));
                } else {
                    PREV[pix_id / 8] &= ~(1 << (pix_id % 8));
                }
                skip --;
            }
        }
    }
    len += len%2;
    return len/2 + BL + 1;
}


uint32_t ptr;
uint32_t counter;
void loop() {

    if(!jay.nextFrame()) return;

    //jay.pollButtons();
    jay.clear();

    //jay.smallPrint(0, 0, "HELLO WORLD");
    //jay.smallPrint(100, 21, itoa(jay.cpuLoad()));


    int ret = drawFrame(FRAMES + ptr);
    counter ++;
    if (counter % 6 == 0) {
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
