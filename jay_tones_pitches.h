#ifndef JAY_TONES_PITCHES
#define JAY_TONES_PITCHES

PROGMEM const uint16_t pitches_7_PGM[] = {
    0, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
};

#define NOTE__ 0
#define NOTE_C 1
#define NOTE_Db 2
#define NOTE_D 3
#define NOTE_Eb 4
#define NOTE_E 5
#define NOTE_F 6
#define NOTE_Gb 7
#define NOTE_G 8
#define NOTE_Ab 9
#define NOTE_A 10
#define NOTE_Bb 11
#define NOTE_B 12

#define NOTE_DUR_2 0
#define NOTE_DUR_4 1
#define NOTE_DUR_8 2
#define NOTE_DUR_6 3

#define SONG_END ((13<<4) | 0x00)
#define SONG_PUSH ((13<<4) | 0x01)
#define SONG_LOOP ((13<<4) | 0x02)

#define SONG_HARMONY ((13<<4) | 0x03)
#define SONG_HARMONY_END SONG_END

#define SONG_LOOP_START(x) \
    ((15<<4) | ((x - 1) & 0xF))

#define TEMPO(t) \
    ((14<<4) | ((t) & 0xF))

#define NOTE(p, o, d) \
    (((NOTE_ ## p) << 4) | ((o & 0x3) << 2) | (NOTE_DUR_ ## d))

#define REST(d) NOTE(_, 0, d)

#endif
