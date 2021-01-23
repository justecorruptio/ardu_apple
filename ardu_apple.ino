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

int drawFrame(uint8_t *frame) {
    uint8_t color = pgm_read_byte(frame + 3) >> 7 & 1;
    int8_t skip = 0;
    uint8_t len = pgm_read_byte(frame + 3) & 0x7F;
    uint8_t ptr = 0;

    for( int y = 0; y < H; y ++) {
        for( int x = 0; x < W; x ++) {
            int pix_id = y * W + x;
            int block_id = y / BS * BW + x / BS;
            if( pgm_read_byte(frame + block_id / 8) & (1 << (block_id % 8)) ) {
                //jay.drawPixel(x, y);
                if (!(PREV[pix_id / 8] & (1 << (pix_id % 8)))) {
                    //jay.drawPixel(x, y);
                    jay.drawFastVLine(x * 4 + 0 + 22, y * 4, 4);
                    jay.drawFastVLine(x * 4 + 1 + 22, y * 4, 4);
                    jay.drawFastVLine(x * 4 + 2 + 22, y * 4, 4);
                    jay.drawFastVLine(x * 4 + 3 + 22, y * 4, 4);
                }
            } else {
                while (!skip) {
                    color = 1 - color;
                    if(ptr >= len) {
                        skip = 255;
                        break;
                    }
                    skip = 0xf & pgm_read_byte(frame + 4 + ptr / 2) >> (4 * (ptr %2));
                    ptr ++;
                }
                if(!color) {
                    //jay.drawPixel(x, y);
                    jay.drawFastVLine(x * 4 + 0 + 22, y * 4, 4);
                    jay.drawFastVLine(x * 4 + 1 + 22, y * 4, 4);
                    jay.drawFastVLine(x * 4 + 2 + 22, y * 4, 4);
                    jay.drawFastVLine(x * 4 + 3 + 22, y * 4, 4);
                    PREV[pix_id / 8] &= ~(1 << (pix_id % 8));
                } else {
                    PREV[pix_id / 8] |= (1 << (pix_id % 8));
                }
                skip --;
            }
        }
    }
    if (len%2) len++;
    return len/2 + 4;
}


uint32_t ptr;
uint32_t counter;
void loop() {

    if(!jay.nextFrame()) return;

    jay.pollButtons();
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
