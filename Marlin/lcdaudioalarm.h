#ifndef lcdaudio_h
#define lcdaudio_h

#include "ultralcd.h"

typedef void (*inactivity_callback_t)();

/**
 * This funcion blocks until the user clicks the ultra LCD button.
 * While waiting it plays a music and periodically executes the inactivity_callback function.
 */
void play_music_until_lcdclick(inactivity_callback_t inactivity_callback);
void stop_playing_music();

#endif

