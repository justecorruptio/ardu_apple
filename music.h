#ifndef MUSIC_H
#define MUSIC_H

PROGMEM const uint8_t bad_apple[] = {
    /*
    TEMPO(6),
    SONG_LOOP_START(3),
        SONG_LOOP_START(3),
            NOTE(A, 0, 4), REST(2), REST(4),
        SONG_LOOP,

        SONG_LOOP_START(4),
            NOTE(A, 0, 8), REST(8),
        SONG_LOOP,

        SONG_LOOP_START(3),
            NOTE(A, 0, 4), REST(2), REST(4),
        SONG_LOOP,

        NOTE(A, 0, 4), REST(4),
        NOTE(A, 0, 4), REST(4),
    SONG_LOOP,

    SONG_LOOP_START(3),
        NOTE(A, 0, 4), REST(2), REST(4),
    SONG_LOOP,
    NOTE(A, 1, 4), REST(2), REST(4),
    */

    TEMPO(12),

    SONG_LOOP_START(3),
        SONG_HARMONY, SONG_PUSH, 1, SONG_HARMONY_END,
        NOTE(A, 0, 4), REST(4), NOTE(A, 0, 4), NOTE(A, 1, 4),
        REST(4), NOTE(A, 1, 4), NOTE(G, 1, 4), NOTE(A, 1, 4),
    SONG_LOOP,

    SONG_HARMONY, SONG_PUSH, 1, SONG_HARMONY_END,

    NOTE(C, 2, 4), REST(4), NOTE(A, 1, 4), NOTE(C, 2, 4),
    NOTE(D, 2, 4), REST(4), NOTE(C, 2, 4), NOTE(D, 2, 4),

    SONG_LOOP_START(3),
        SONG_HARMONY, SONG_PUSH, 1, SONG_HARMONY_END,
        NOTE(A, 0, 4), REST(4), NOTE(A, 0, 4), NOTE(A, 1, 4),
        REST(4), NOTE(A, 1, 4), NOTE(G, 1, 4), NOTE(A, 1, 4),
    SONG_LOOP,

    SONG_HARMONY, SONG_PUSH, 1, SONG_HARMONY_END,

    NOTE(D, 2, 4), REST(4), NOTE(C, 2, 4), NOTE(D, 2, 4),
    NOTE(C, 2, 4), REST(4), NOTE(A, 1, 4), NOTE(C, 2, 4),


    SONG_END,
};

PROGMEM const uint8_t rumble [] = {
    SONG_LOOP_START(4),
        NOTE(A, 0, 8), REST(8), NOTE(C, 0, 8), REST(8),
    SONG_LOOP,
    SONG_END,
};

PROGMEM const uint8_t* const music [] = {
    bad_apple,
    rumble,
};

/*
PROGMEM const uint8_t song1[] = {
    TEMPO(12),

    SONG_LOOP_START(2),
        SONG_LOOP_START(3),
            NOTE(G, 1, 4), REST(4), NOTE(G, 1, 4), REST(4),
            NOTE(G, 1, 4), REST(4), NOTE(G, 1, 4), NOTE(F, 1, 4),
        SONG_LOOP,

        NOTE(G, 1, 4), NOTE(Bb, 1, 4), NOTE(G, 1, 4), NOTE(C, 2, 4),
        NOTE(G, 1, 4), NOTE(Db, 2, 4), NOTE(G, 1, 4), NOTE(F, 1, 4),
    SONG_LOOP,

    NOTE(G, 2, 4), NOTE(Bb, 2, 4), NOTE(A, 2, 4), NOTE(C, 3, 4),
    NOTE(B, 2, 4), NOTE(D, 3, 4), NOTE(Db, 3, 4), NOTE(E, 3, 4),

    //Chorus

    SONG_LOOP_START(4), NOTE(D, 3, 6), NOTE(B, 2, 6), NOTE(Gb, 2, 6), SONG_LOOP,
    SONG_LOOP_START(4), NOTE(E, 3, 6), NOTE(Db, 3, 6), NOTE(Gb, 2, 6), SONG_LOOP,
    SONG_LOOP_START(4), NOTE(Gb, 3, 6), NOTE(D, 3, 6), NOTE(B, 2, 6), SONG_LOOP,
    SONG_LOOP_START(4), NOTE(G, 3, 6), NOTE(D, 3, 6), NOTE(B, 2, 6), SONG_LOOP,

    SONG_LOOP_START(4), NOTE(G, 3, 6), NOTE(Eb, 3, 6), NOTE(B, 2, 6), SONG_LOOP,
    SONG_LOOP_START(4), NOTE(G, 3, 6), NOTE(E, 3, 6), NOTE(B, 2, 6), SONG_LOOP,
    SONG_LOOP_START(4), NOTE(G, 3, 6), NOTE(E, 3, 6), NOTE(C, 3, 6), SONG_LOOP,

    NOTE(B, 2, 4), NOTE(B, 1, 4), NOTE(Db, 2, 4), NOTE(Eb, 2, 4),
    NOTE(E, 2, 4), NOTE(Eb, 2, 4), NOTE(E, 2, 4), NOTE(Gb, 2, 4),
    NOTE(G, 2, 4), NOTE(Gb, 2, 4), NOTE(G, 2, 4), NOTE(A, 2, 4),
    NOTE(B, 2, 4), NOTE(A, 2, 4), NOTE(C, 3, 4), NOTE(B, 2, 4),

    NOTE(B, 2, 4), NOTE(A, 2, 4),
    NOTE(G, 2, 4), NOTE(Gb, 2, 4),
    TEMPO(10),
    NOTE(E, 2, 4), NOTE(D, 2, 4),
    TEMPO(8),
    NOTE(C, 2, 4), NOTE(F, 1, 4),

    SONG_REPEAT
};

PROGMEM const uint8_t song2[] = {
    TEMPO(4),

    SONG_HARMONY,
        NOTE(C, 1, 2), NOTE(C, 1, 2),
        REST(4), NOTE(B, 0, 8), NOTE(C, 1, 8), NOTE(D, 1, 4), NOTE(C, 1, 8), NOTE(B, 0, 8),
        NOTE(A, 0, 2), NOTE(A, 0, 2),
        REST(4), NOTE(Ab, 0, 8), NOTE(A, 0, 8), NOTE(B, 0, 4), NOTE(Ab, 0, 4),
    SONG_HARMONY_END,

    REST(4), NOTE(C, 2, 8), NOTE(D, 2, 8), NOTE(E, 2, 4), NOTE(C, 2, 4),
    NOTE(G, 2, 2), NOTE(G, 2, 2),
    REST(4), NOTE(A, 1, 8), NOTE(B, 1, 8), NOTE(C, 2, 4), NOTE(A, 1, 4),
    NOTE(E, 2, 2), NOTE(E, 2, 2),

    SONG_HARMONY,
        NOTE(F, 0, 2), NOTE(F, 0, 2),
        NOTE(E, 0, 2), NOTE(E, 0, 2),
        REST(4), NOTE(G, 0, 8), NOTE(A, 0, 8), NOTE(B, 0, 8), NOTE(C, 1, 8), NOTE(D, 1, 8), NOTE(C, 1, 8),
        NOTE(D, 1, 8), NOTE(E, 1, 8), NOTE(F, 1, 8), NOTE(G, 1, 8), NOTE(A, 1, 8), NOTE(B, 1, 8),
    SONG_HARMONY_END,

    REST(4), NOTE(C, 2, 4), NOTE(D, 2, 4), NOTE(E, 2, 4),
    NOTE(G, 2, 4), NOTE(F, 2, 4), NOTE(C, 2, 4), NOTE(Ab, 1, 4),
    NOTE(G, 1, 2), NOTE(G, 1, 2),
    NOTE(G, 1, 2), NOTE(G, 1, 4),

    SONG_REPEAT
};
*/

#endif
