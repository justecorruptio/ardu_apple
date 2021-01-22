#include "jaylib.h"
#include "utils.h"
#include "frames.h"

Jaylib jay;

void setup() {
    jay.boot();
    jay.invert(1);
    jay.clear();
}

#define W 21
#define H 16

#define BS 4
#define BW 6

uint8_t PREV [W * H / 8];


void loop() {

    if(!jay.nextFrame()) return;

    jay.pollButtons();
    jay.clear();

    //jay.smallPrint(0, 0, "HELLO WORLD");
    //jay.smallPrint(100, 21, itoa(jay.cpuLoad()));

    uint8_t *frame = FRAMES;
    uint8_t color = pgm_read_byte(frame + 3) >> 7 & 1;
    int8_t skip = (pgm_read_byte(frame + 3) & 0x7f);
    uint8_t len = pgm_read_byte(frame + 4);
    uint8_t ptr = 0;

    jay.smallPrint(100, 21, itoa(len));

    for( int y = 0; y < H; y ++) {
        for( int x = 0; x < W; x ++) {
            int pix_id = y * W + x;
            int block_id = y / BS * BW + x / BS;
            if( pgm_read_byte(frame + block_id / 8) & (1 << (block_id % 8)) ) {
                //jay.drawPixel(x, y);
                if (!(PREV[pix_id / 8] & (1 << (pix_id % 8)))) {
                    jay.drawPixel(x, y);
                }
            } else {
                while (!skip) {
                    color = 1 - color;
                    if(ptr >= len) {
                        skip = 255;
                        break;
                    }
                    skip = 0xf & pgm_read_byte(frame + 5 + ptr / 2) >> (4 * (ptr %2));
                    ptr ++;
                }
                if(!color)
                    jay.drawPixel(x, y);
                skip --;
            }
        }
    }

    jay.display();
}

// vim:syntax=c
