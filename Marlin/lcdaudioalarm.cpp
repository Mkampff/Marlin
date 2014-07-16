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
int melody[] = { c, R};
int beats[]  = {16,16};
int MAX_COUNT = sizeof(melody) / sizeof(int);

#define TEMPO_MICROS 50000
#define PAUSE_MICROS 10000
#define REST_COUNT 100

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

void play_music_until_lcdclick(inactivity_callback_t inactivity_callback) {
    #if BEEPER > 0
        int beat = 0;
        int tone = 0;
        long duration  = 0;

        // Set beeper pin as OUT
        SET_OUTPUT(BEEPER);
        
        while(!lcd_clicked()){
            for (int i = 0; i < MAX_COUNT && !lcd_clicked(); i++) {
                // periodic activies
                inactivity_callback();

                tone = melody[i];
                beat = beats[i];

                duration = beat * TEMPO_MICROS;

                playTone(duration, tone);

                // delay between two beats
                delayMicroseconds(PAUSE_MICROS);
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