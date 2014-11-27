#include "lcdaudioalarm.h"

/*
 * based on http://www.arduino.cc/en/Tutorial/PlayMelody
 * Original source code written by:
 * (cleft) 2005 D. Cuartielles for K3
 * Refactoring and comments 2006 clay.shirky@nyu.edu
 */

#define  c     477    // 262 Hz
#define  d     425    // 294 Hz
#define  e     379    // 330 Hz
#define  f     358    // 349 Hz
#define  g     319    // 392 Hz
#define  a     284    // 440 Hz
#define  b     253    // 494 Hz
#define  C     239    // 523 Hz
#define  R     0

//int melody[] = { e, f, g, g, f, e, d, c, c, d, e, e, d};
//int beats[]  = {16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
static int melody[] = { c, R};
static int beats[]  = {16,16};
static int MAX_COUNT = sizeof(melody) / sizeof(int);
static bool continue_playing;

#define TEMPO_MICROS 50000
#define PAUSE_MICROS 10000
#define REST_COUNT 100
#define IDLE_CHECK_COUNT 4

#if BEEPER > 0
inline void playTone(long duration, int tone) {
    if (tone != R) {
        long elapsed_time = 0;
        while (elapsed_time < duration) {
            WRITE(BEEPER, HIGH);
            delayMicroseconds(tone / 2);
            WRITE(BEEPER, LOW);
            delayMicroseconds(tone / 2);

            elapsed_time += (tone);
        }
    } else {
        // Rest beat. Repeat (rest delays are much slower)
        for (int j = 0; j < REST_COUNT; j++) {
            delayMicroseconds(duration);
        }
    }
}
#endif

void stop_playing_music() {
    // force the code out of the beeper loop
    continue_playing = false;
}

void play_music_until_lcdclick(inactivity_callback_t inactivity_callback) {
    #if BEEPER > 0
        int beat = 0;
        int tone = 0;
        long duration  = 0;
        continue_playing = true;

        // Set beeper pin as OUT
        SET_OUTPUT(BEEPER);
        
        while(continue_playing){
            for (int i = 0; i < MAX_COUNT && continue_playing; i++) {
                // periodic activies
                inactivity_callback();

                tone = melody[i];
                beat = beats[i];

                duration = beat * TEMPO_MICROS;

                for (int j = 0; j < IDLE_CHECK_COUNT; j++) {
                    playTone(duration / IDLE_CHECK_COUNT, tone);
                    continue_playing = continue_playing && !lcd_clicked(); if (!continue_playing) break;
                }

                // delay between two beats
                for (int j = 0; j < IDLE_CHECK_COUNT; j++) {
                    delayMicroseconds(PAUSE_MICROS / IDLE_CHECK_COUNT);
                    continue_playing = continue_playing && !lcd_clicked(); if (!continue_playing) break;
                }
            }
        }
    #else
        // no music, just frequencies
        #if !defined(LCD_FEEDBACK_FREQUENCY_HZ) || !defined(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
            lcd_buzz(1000/6,100);
        #else
            lcd_buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
    #endif
}