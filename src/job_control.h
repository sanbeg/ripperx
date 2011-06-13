
#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

#include "common.h"

enum JC 
  {

    JC_START,
    JC_UPDATE,
    JC_PAUSE,
    JC_CONT,
    JC_ABORT,
    JC_ABORT_DELETE,
    JC_ABORT_ALL,
    JC_ABORT_ALL_DELETE
  };

  
const int JC_TIMEOUT = 450;

void job_starter(_main_data *main_data);

void job_controller(int ops, _main_data *main_data);
/* Job controller. This function saves main_data
 * when called with JC_START and uses it for further operation */

#endif
