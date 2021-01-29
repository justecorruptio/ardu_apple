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

#define JUMP (FRAMES_JUMP * 2)

#define FACTOR (64 / FRAMES_H)
#define OFFX ((128 - W * FACTOR) / 2)
#define OFFY ((64 - H * FACTOR) / 2)

uint8_t PREV [W * H / 8];

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
                    skip = 0xf & pgm_read_byte(frame + BL + 2 + ptr / 2) >> (4 * (ptr %2));
                    ptr ++;
                    if(skip == 0) {
                        skip = 0xf & pgm_read_byte(frame + BL + 2 + ptr / 2) >> (4 * (ptr %2));
                        ptr++;
                        q = 4;
                        while (1) {
                            p = 0xf & pgm_read_byte(frame + BL + 2 + ptr / 2) >> (4 * (ptr %2));
                            ptr++;
                            skip |= (p & 0x7) << q;
                            if (!(p & 0x8)) {
                                break;
                            }
                            q += 3;
                        }
                    }
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

    len += len%2;
    return len/2 + BL + 2;
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
