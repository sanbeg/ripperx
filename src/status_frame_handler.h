#ifndef STATUS_FRAME_HANDLER_H
#define STATUS_FRAME_HANDLER_H

#include "config.h"

#include "common.h"

#include <gtk/gtk.h>

void wm_status_frame_handler(int ops, int type,
                             _stat *stat, char *graph_string);

#endif
