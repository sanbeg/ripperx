/* file common.h
 *
 * defines constants common to all files in ripperx
 *
 * TODO: move the includes to the files where they are used (ralf)
 *
 * Ralf Engels  10/06/1999  commented extern int errno out
 *                          moved include <errno> to files where needed
 *                          removed all the warnings
 */
#ifndef COMMON_H
#define COMMON_H

#include "../config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include "err_dialog_handler.h"

#ifndef TRUE
#define TRUE		1
#endif
#ifndef FALSE
#define FALSE		0
#endif

/* =====================================================
 * Constants
 * ===================================================*/

/* Global constants */
#define MAX_OPTIONS_LENGTH	300
#define MAX_SINGLE_OPTION_LENGTH	50

#define MAX_COMMAND_LENGTH	2048

#define MAX_ARGS		50

#define MAX_FILE_PER_DIR	1024
#define MAX_FILE_NAME_LENGTH	512
#define MAX_FILE_PATH_LENGTH	512
#define MAX_TOTAL_PATH_LENGTH	(MAX_FILE_PATH_LENGTH+MAX_FILE_NAME_LENGTH)
#define MAX_NUM_TRACK		100
#define MAX_TITLE_LENGTH	1024
#define MAX_ARTIST_LENGTH	1024
#define MAX_YEAR_LENGTH		30
#define MAX_GENRE_LENGTH	1024
/* MAX_CDDB_LENGTH is the maximum length of a CDDB element */
#define MAX_CDDB_LENGTH		1024

#define CD_SECTORS_PER_SEC	(float)75.018
#define CD_SECTORS_PER_FRAME 	(float)1.959

enum CommonEncoderType 
  {
    CD,
    WAV,
    MP3,
    OGG,
    FLAC,
    MP2,
    MUSE
  }
  ;

#define MIN_NICE_LEVEL    19
#define MAX_NICE_LEVEL    0


/* =====================================================
 * Structures
 * ===================================================*/

/* Configuration Structure */
typedef struct
{
    float wav_ratio;
    float mp3_ratio;
    char shell_for_execution[ MAX_COMMAND_LENGTH ];
    char wav_path[ MAX_FILE_PATH_LENGTH ];
    char mp3_path[ MAX_FILE_PATH_LENGTH ];
    char cddb_path[ MAX_FILE_PATH_LENGTH ];
    char wav_file_name_format[ MAX_FILE_NAME_LENGTH ];
    char mp3_file_name_format[ MAX_FILE_NAME_LENGTH ];
    char prepend_char;
    int make_mp3_from_existing_wav;
    int ask_when_file_exists;
    int auto_append_extension;
    int keep_wav;

    struct _ripper
    {
        char ripper[ MAX_COMMAND_LENGTH ];
        char plugin[ MAX_COMMAND_LENGTH ];
    } ripper;
    struct _encoder
    {
        char encoder[ MAX_COMMAND_LENGTH ];
        char plugin[ MAX_COMMAND_LENGTH ];
        int type;
        int bitrate;
        int use_varbitrate;
        int priority;
        int use_high_qual;
        int use_crc;
        int vbr_qual;
        char extra_options[ MAX_OPTIONS_LENGTH ];
        char full_command[ MAX_COMMAND_LENGTH ];
    } encoder;
    struct _cd_player
    {
        char play_command[ MAX_COMMAND_LENGTH ];
        char stop_command[ MAX_COMMAND_LENGTH ];
    } cd_player;
    struct _wav_player
    {
        char command[ MAX_COMMAND_LENGTH ];
    } wav_player;
    struct _mp3_player
    {
        char command[ MAX_COMMAND_LENGTH ];
    } mp3_player;
    struct _cddb_config
    {
        char server[ MAX_COMMAND_LENGTH ];
        int port;
        int use_http;
        int convert_spaces;
        int make_directories;
        int create_id3;
        int create_playlist;
        int auto_lookup;
        char format_string[ MAX_ARTIST_LENGTH ];
        char dir_format_string[ MAX_ARTIST_LENGTH ];
        char proxy_server[ MAX_COMMAND_LENGTH ];
        int proxy_port;
    } cddb_config;
} _config;

/* Main data structure             */
/* All length is in cd sector unit */
typedef struct
{
    int num_tracks;
    int total_length;
    char disc_artist[ MAX_ARTIST_LENGTH ];
    char disc_title[ MAX_ARTIST_LENGTH ];
    char disc_year[ MAX_YEAR_LENGTH ];
    char disc_category[ MAX_ARTIST_LENGTH ];
    struct _track
    {
        char title[ MAX_FILE_NAME_LENGTH ];
        unsigned begin;
        unsigned length;
        int make_wav;
        int wav_exist;
        int make_mp3;
        int mp3_exist;
        char *artist;
    } track[ MAX_NUM_TRACK ];
} _main_data;

typedef struct
{
    int wav_track;
    int mp3_track;
    char src_file_name[ MAX_FILE_NAME_LENGTH ];
    char dest_file_name[ MAX_FILE_NAME_LENGTH ];
    time_t wav_time_elapsed;
    time_t mp3_time_elapsed;
    time_t wav_time_remain;
    time_t mp3_time_remain;
    time_t total_time_elapsed;
    time_t total_time_remain;
    float wav_progress;
    float mp3_progress;
    float total_progress;
    int tracks_done;
    int tracks_remain;
    int ripping;
    int encoding;
} _stat;

extern _config config;

enum CommonLocation 
  {
    SELECT_FRAME,
    STATUS_FRAME,
    CONFIG_WINDOW
  };

extern enum CommonLocation where_now;

#endif
