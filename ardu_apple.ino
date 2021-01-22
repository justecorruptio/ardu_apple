#include "jaylib.h"
#include "utils.h"

Jaylib jay;

void setup() {
    jay.boot();
    jay.invert(1);
    jay.clear();
}

void loop() {

    if(!jay.nextFrame()) return;

    jay.pollButtons();
    jay.clear();

    jay.smallPrint(0, 0, "HELLO WORLD");
    jay.smallPrint(100, 21, itoa(jay.cpuLoad()));

    jay.display();
}

// vim:syntax=c
