#ifndef MISC_UTILS_H
#define MISC_UTILS_H

#include "../config.h"

#include "common.h"

#include <stdio.h>

#include <id3.h>

enum MiscUtils 
  {
    MISC_OK,
    MISC_NOT_DIR,
    MISC_NOT_WRITABLE,
    MISC_DOES_NOT_EXISTS
  };

/* contents immigrated from misc.h */

char *get_string_piece(FILE *file, int delim);
char *get_ascii_file(FILE *file);
void strip_trailing_space(char **string);
void strip_leading_space(char **string);
char *string_append(char **dest, char *appendage);
void charpp_to_charp(char **dest, char **src, int num, char *separator);
FILE *socket_init(const char *server, short int port);
char *int2str(int integer);
int get_subdirs(const char *path, char **buffer);

/* end of misc.h */

int add_argv(char **dest, char *content);
int process_options(char *options, char **argv,
                    int start, int end);

int execute_using_shell(const char *command) ;
int parse_rx_format_string(char **target,
                           char *format,
                           int track_no, const char *artist, const char *album,
                           const char *year, const char *song);
// track_no starts from 0
// %% %
// %a Artist
// %v album (Volume)
// %y Year
// %s Song

char *length_to_readable(unsigned length);
char *time_to_readable(time_t sec);
char *construct_file_name(const char *path, const char *name);
char *expand_tilde(char *path);
char *file_name_without_path(char *src);
char *file_path_without_name(char *src);
void auto_append_extension(char *src, int type);
char *get_default_track_title(int track);
void remove_non_unix_chars(char *src);
void convert_slashes(char *src, char c);
void convert_spaces(char *src, char c);

int is_str_blank(char *str);
long check_free_space(char *dir);
int check_dir(char *dir);
int create_dir(char *path);
int is_found(const char *plugin);
int create_filenames_from_format(_main_data *main_data);
int create_file_names_for_track(const _main_data *main_data, int track, char **wfp, char **efp);
void get_track_title(char *dest, _main_data *main_data, int tno);
void put_track_title(char *src, _main_data *main_data, int tno);
char *dup_str(char *inp);
void mk_buf(char **ptr, int size);
void mk_str(char **ptr, char *inp);
void mk_strcat(char **ptr, ...);
void set_TagField(ID3Tag *myTag, char *data, ID3_FrameID id);
unsigned char id3_find_cddb_category(char *name);
void vorbistag(char *ogg_file, char *artist, char *album, char *date,
               char *title, unsigned char style, unsigned char track);
void flactag(char *flac_file, char *artist, char *album, char *date,
             char *title, unsigned char style, unsigned char track);
#endif
