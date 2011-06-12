#ifndef RIPPER_ENCODER_MANIPULATION_H
#define RIPPER_ENCODER_MANIPULATION_H

#include "common.h"

#define BUF_LENGTH_FOR_F_SCAN_CD    150

#define MAX_PLUGIN_OUTPUT_LENGTH    1024

// return values of read_and_process... & parse_plugin_output
enum Plugin 
  {
    PLUGIN_MSG_PARSE_ERR = -2,
    PLUGIN_NO_MSG_AVAILABLE = -1,
    PLUGIN_PROGRESS_MSG = 0,
    PLUGIN_WARN_MSG = 1,
    PLUGIN_ERR_MSG = 2
  };
  

int scan_cd(_main_data *main_data);

int start_ripping_encoding(int type, int begin, int length,
                           int track,
                           char *src_file_name, char *dest_file_name,
                           int *program_pid, int *plugin_pid,
                           int *read_fd);
// type is either WAV(rip) or MP3(encode)

int read_and_process_plugin_output(int read_fd, double *progress, char *msg);

#endif
