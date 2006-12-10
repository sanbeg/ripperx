
#ifndef PLAYERS_MANIPULATION_H
#define PLAYERS_MANIPULATION_H

#include "common.h"

#define PLAY       0
#define STOP       1

char **players_create_argv( int ops, int cd_wav_mp3, char *playit );

int play_cd_wav_mp3( int ops, int cd_wav_mp3, char *playit );
/* The player. This function return TRUE on success. FALSE on failure.
 * This function uses players_create_argv function.
 * ops selects to play or to stop. playit
 * is a string which contains the number of track if ops is CD(that is "3". 
 * Otherwise, it should contain the name of file to play */

#endif
