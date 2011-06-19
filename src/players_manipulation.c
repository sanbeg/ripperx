#include "config.h"

#include <stdio.h>

#ifdef HAVE_UNISTD_H
# include <sys/types.h>
# include <unistd.h>
#endif

#include <signal.h>

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned int) (stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif

#include "misc_utils.h"
#include "players_manipulation.h"

#include <glib.h>
#include <glib/gi18n.h>

namespace 
{
  char *players_create_argv(enum PlayerOps ops, enum CommonEncoderType cd_wav_mp3, const char *playit)
  {
    int i, j, d;
    char *buf = new char[ MAX_COMMAND_LENGTH ];
    char *command = NULL;
    
    switch(cd_wav_mp3)
      {
      case CD :
	
	if(ops == PLAY)
	  {
	    command = config.cd_player.play_command;
	  }
	else
	  {
	    command = config.cd_player.stop_command;
	  }
	
	break;
      case WAV :
	command = config.wav_player.command;
	break;
      case MP3 :
	command = config.mp3_player.command;
	break;
      }
    
    // expand '%'
    strcpy(buf, command);
    
    for(i = 0, d = 0; command[ i ] != '\0';)
      if(command[ i ] == '%')
        {
	  for(j = 0; playit[ j ] != '\0';)
            {
	      buf[ d++ ] = playit[ j++ ];
            }
	  
	  i++;
        }
      else
        {
	  buf[ d++ ] = command[ i++ ];
        }
    
    buf[ d ] = '\0';
    return buf;
  }
}

int play_cd_wav_mp3(enum PlayerOps ops, enum CommonEncoderType cd_wav_mp3, const char *playit)
{
    char *arg;
    static int null_fd, stderr_fd;
    pid_t pid;
    static pid_t saved_pid;
    static int is_first_time = 1;

    if(is_first_time)
    {
        /* Open /dev/null */
        if((null_fd = open("/dev/null", O_WRONLY)) < 0)
        {
            err_handler(NULL_OPEN_ERR, NULL);
            return FALSE;
        }

        is_first_time = 0;
    }

    while(waitpid(-1, NULL, WNOHANG) > 0) ;

    /* Create appropriate argvs */
    if(ops == PLAY || cd_wav_mp3 == CD)
    {
        if((arg = players_create_argv(ops, cd_wav_mp3, playit)) == NULL)
        {
            return FALSE;
        }

        /* Fork */
        if((pid = fork()) < 0)
        {
            err_handler(FORK_ERR, NULL);
            delete [] arg;
            return FALSE;
        }

        if(pid == 0)
        {
            /* This code will be excuted in the child process */
            /* Throw away stdout and stderr */
            stderr_fd = dup(2);
            dup2(null_fd, 2);
            dup2(null_fd, 1);

            /* Execute the player */
	    execute_using_shell(arg);
	    
            dup2(stderr_fd, 2);
            perror(_("Failed to exec player :"));
            _exit(127);
        }

        if(ops == PLAY)
        {
            saved_pid = pid;
        }

	delete [] arg;
	
        return TRUE;
    }
    else
    {
        if(saved_pid > 0)
            if(waitpid(saved_pid, NULL, WNOHANG) == 0)
            {
                kill(saved_pid, SIGTERM);
                waitpid(saved_pid, NULL, 0);
            }

        pid = -1;
        return TRUE;
    }
}
