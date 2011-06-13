#ifndef SELECT_FRAME_HANDLER_H
#define SELECT_FRAME_HANDLER_H

#include "config.h"

#include "common.h"

#include <gtk/gtk.h>

enum SelectFrameHandler 
  {
    SF_SELECT_BUTTON_ACT = 101,
    SF_ALBUM_ENTRY_CHG,
    SF_FILENAME_ENTRY_CHG,
    SF_ARTIST_ENTRY_CHG,
    SF_SELECT_BUTTON_ACT_ALL,
    SF_SYNC_MAIN_DATA,
    SF_SYNC_SELECT_FRAME,
    SF_SELECT_ALL,

    SF_YEAR_ENTRY_CHG,
    SF_GENRE_ENTRY_CHG
  };

  

/* Create or destroy select frame. ops can be one of WIDGET_CREATE,
 * WIDGET_DESTROY, SF_WAV_BUTTON_ACT, SF_MP3_BUTTON_ACT,
 * , SF_WAV_ENTRY_CHG, SF_MP3_ENTRY_CHG, and SF_SYNC_MAIN_DATA.
 * When called with WIDGET_CREATE, it will save the value of *main_data
 * and use it for further operation until the widget is created again.
 * So, it's important not to change *main_data while this function is
 * operating on it */
/* Call back functions will convert gpointer callback_data to an integer */
void select_frame_handler(int ops, int track, _main_data *main_data);

#endif
