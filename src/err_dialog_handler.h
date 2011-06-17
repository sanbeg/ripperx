
#ifndef ERR_DIALOG_HANDLER_H
#define ERR_DIALOG_HANDLER_H

#include "common.h"
#include <stdexcept>

enum ErrorCodes 
  {
    /* Error codes */
    FORK_ERR,
    PIPE_CREATION_ERR,
    NULL_OPEN_ERR,
    MALLOC_ERR,
    FDOPEN_ERR,
    PIPE_READ_ERR,
    FILE_DELETE_ERR,
    CONFIG_OPEN_ERR,
    CONFIG_CREATION_ERR,
    PTY_OPEN_ERR,
    END_PERROR,

    TOO_MANY_ARGS_ERR,
    JOB_IN_PROGRESS_ERR,
    NOTHING_TO_DO_ERR,
    CD_PARANOIA_MISC_ERR,
    TOO_LONG_FILE_NAME_ERR,
    
    EMPTY_ENTRY_ERR,
    CONFIG_PARSE_ERR,
    CONFIG_EMPTY_ITEM_ERR,
    
    INVALID_FILE_SELECTION_ERR,

    /* CDDB Error Codes */
    CDDB_NO_CONNECT_ERR,
    CDDB_CONNECT_REFUSED_ERR,
    CDDB_SERVER_ERR,
    CDDB_NOT_FOUND_ERR,

    /* Newly added ones */
    RX_PARSING_ERR,
    PLUGIN_NOT_PRESENT_ERR,
    CREATING_FILE_ERROR,
    WAV_PATH_NOT_WRITABLE_ERROR,
    MP3_PATH_NOT_WRITABLE_ERROR,
    WAV_PATH_CREATE_ERROR,
    MP3_PATH_CREATE_ERROR,

    /* more distinct cdparanoia error codes */
    CD_PARANOIA_NO_DISC,
    CD_PARANOIA_NO_PERMS,
    /* catch any exceptions */
    CPP_EXCEPTION_ERR
  };

enum DialogCodes 
  {
    
    /* Dialog codes */
    DL_ABORT_CONFIRM,
    DL_DELETE_ON_ABORT,
    DL_OVERWRITE_CONFIRM,
    DL_ENTER_FILE_NAME,
    DL_CREATE_CONFIG_CONFIRM,
    DL_WAV_PART_LOW,
    DL_MP3_PART_LOW
  };

enum StatusCodes 
  {
    /* Status codes */
    STAT_FINISH_WAV,
    STAT_FINISH_MP3,
    
    /* For dialog handler */
    DL_OK_PRESSED			=	100
  };


/* Function Prototypes */
class ErrCode 
{
 public:
 ErrCode(int x) :err_code(x)
  {
    *buf=0;
  }
  
  virtual const char * get_buf() const;
  virtual const char * get_msg() const;
  virtual bool has_perror() const;
 private:
  int err_code;
  mutable char buf[20];
};

class ExcCode : public ErrCode 
{
 public:
  
 ExcCode(std::exception &x) : ErrCode(CPP_EXCEPTION_ERR), e(x) 
  {
  }
  ;
  
  const char * get_buf() const;
  const char * get_msg() const;
  bool has_perror() const;

 private:
  std::exception & e;
};



void err_handler(const ErrCode &, const char *extra_msg);

int dialog_handler(int ops, int ok_or_yes, int dialog_code,
                   int with_entry, char *entry_default,
                   char *entered, int answer_length);

void status_handler(int status_code, const char *extra_msg);
#endif
