#include "jaylib.h"
#include "music.h"

static bool enabled = false;

typedef struct {
    uint8_t loops;
    uint8_t * loop_start;
} song_frame_t;

typedef struct {
    int32_t dur;
    uint16_t note_dur[4] = {300, 150, 75, 100}; // SPACE
    uint8_t * notes;
    uint8_t * ptr;
    uint8_t playing = 1;
    song_frame_t stack[4];
    uint8_t s_idx = 0;
} song_t;

static song_t songs[2];

void JayTones::on() {
    enabled = true;
    power_timer1_enable();
    power_timer3_enable();
}

void JayTones::off() {
    enabled = false;
    power_timer1_disable();
    power_timer3_disable();
}

void JayTones::init() {
    pinMode(PIN_SPEAKER_1, OUTPUT);
    pinMode(PIN_SPEAKER_2, OUTPUT);
    TCCR1A = (1 << WGM10) | (1 << WGM11);
    TCCR1B = (1 << WGM12) | (1 << WGM13);
    TCCR3A = (1 << WGM30) | (1 << WGM31);
    TCCR3B = (1 << WGM32) | (1 << WGM33);
}

void JayTones::playNote(uint8_t chan, uint16_t freq, int16_t dur) {
    song_t * song = songs + chan;

    if(freq)
        song->playing = 1;
    else {
        freq = 440;
        song->playing = 0;
    }

    uint32_t ocr = F_CPU / freq;
    byte bits = 0x01;
    if(ocr > 0xFFFF) {
        ocr >>= 3;
        bits = 0x02;
    }
    if(ocr > 0xFFFF) {
        ocr >>= 3;
        bits = 0x03;
    }

    song->dur = ((uint32_t)dur * freq) >> 9;

    if(chan) {
        TCCR1B = TCCR1B & 0xF8 | bits;
        OCR1A = ocr - 1;
        OCR1B = ocr >> 1;
        TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B) ;
    } else {
        TCCR3B = TCCR3B & 0xF8 | bits;
        OCR3A = ocr - 1;
        OCR3B = ocr >> 1;
        TIMSK3 |= (1 << OCIE3A) | (1 << OCIE3B) ;
    }
}

void JayTones::playSong(uint8_t chan, uint8_t * notes){
    song_t * song = songs + chan;
    song->notes = notes;
    song->ptr = notes;
    song->s_idx = 0;
    nextNote(chan);
}

void JayTones::stopSong(uint8_t chan) {
    if(chan) TIMSK1 = 0;
    else TIMSK3 = 0;
}

void JayTones::nextNote(uint8_t chan){
    song_t * song = songs + chan;
    song_frame_t * frame;

    lbl_nextNote:

    uint8_t data = pgm_read_byte(song->ptr);
    uint8_t pitch_idx = data >> 4;

    if(pitch_idx == 13) {
        switch(data & 0xF) {
            case 1:
                frame = song->stack + (song->s_idx ++);
                frame->loop_start = song->ptr + 2;
                frame->loops = 1;
                song->ptr = pgm_read_ptr(music + pgm_read_byte(song->ptr + 1));
                goto lbl_nextNote;
            case 0:
                if (song->s_idx == 0) {
                    stopSong(chan);
                    return;
                } else {
                    frame = song->stack + --song->s_idx;
                    song->ptr = frame->loop_start;
                    goto lbl_nextNote;
                }
            case 2:
                frame = song->stack + song->s_idx - 1;
                if(--frame->loops > 0)
                    song->ptr = frame->loop_start;
                else {
                    song->s_idx --;
                    song->ptr ++;
                }
                goto lbl_nextNote;
            case 3:
                song_t * other = songs + (1 - chan);
                other->note_dur[0] = song->note_dur[0];
                other->note_dur[1] = song->note_dur[1];
                other->note_dur[2] = song->note_dur[2];
                other->note_dur[3] = song->note_dur[3];

                playSong(1 - chan, song->ptr + 1);

                while(pgm_read_byte(song->ptr) != SONG_HARMONY_END)
                    song->ptr ++;
                song->ptr ++;
                goto lbl_nextNote;
        }
    }

    else if(pitch_idx == 14) {
        uint16_t tempo = ((data & 0xF) << 0);
        song->note_dur[3] = tempo << 2;
        tempo *= 3;
        song->note_dur[0] = tempo << 2;
        song->note_dur[1] = tempo << 1;
        song->note_dur[2] = tempo;

        song->ptr ++;
        goto lbl_nextNote;
    }
    else if(pitch_idx == 15) {
        song->ptr ++;
        frame = song->stack + (song->s_idx ++);
        frame->loop_start = song->ptr;
        frame->loops = (data & 0xF) + 1;
        goto lbl_nextNote;
    }

    uint16_t freq = pgm_read_word(pitches_7_PGM + pitch_idx);
    freq >>= 5 - ((data & 0x0C) >> 2);
    int16_t dur = song->note_dur[data & 0x3];
    song->ptr ++;
    playNote(chan, freq, dur);
}

ISR(TIMER3_COMPA_vect) {
    if(songs[0].dur --) {
        if(songs[0].playing) *(&PORTC) ^= (1 << PORTC6);
    }
    else
        JayTones::nextNote(0);
}

ISR(TIMER3_COMPB_vect) {
    if(songs[0].playing) *(&PORTC) ^= (1 << PORTC6);
}

ISR(TIMER1_COMPA_vect) {
    if(songs[1].dur --) {
        if(songs[1].playing) *(&PORTC) ^= (1 << PORTC7);
    }
    else
        JayTones::nextNote(1);
}

ISR(TIMER1_COMPB_vect) {
    if(songs[1].playing) *(&PORTC) ^= (1 << PORTC7);
}
