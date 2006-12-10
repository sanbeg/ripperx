
#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

#include "common.h"

#define JC_START                    0
#define JC_UPDATE                   1
#define JC_PAUSE                    2
#define JC_CONT                     3
#define JC_ABORT                    4
#define JC_ABORT_DELETE             5
#define JC_ABORT_ALL                6
#define JC_ABORT_ALL_DELETE         7
#define JC_TIMEOUT                  450

void job_starter( _main_data *main_data );

void job_controller( int ops, _main_data *main_data );
/* Job controller. This function saves main_data
 * when called with JC_START and uses it for further operation */

#endif
