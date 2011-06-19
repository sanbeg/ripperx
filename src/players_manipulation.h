#ifndef PLAYERS_MANIPULATION_H
#define PLAYERS_MANIPULATION_H

#include "common.h"

enum PlayerOps 
  {
    PLAY,
    STOP
  };

  
int play_cd_wav_mp3(enum PlayerOps ops, 
		    enum CommonEncoderType cd_wav_mp3, 
		    const char *playit);
/* The player. This function return TRUE on success. FALSE on failure.
 * This function uses players_create_argv function.
 * ops selects to play or to stop. playit
 * is a string which contains the number of track if ops is CD(that is "3".
 * Otherwise, it should contain the name of file to play */

#endif
