#ifndef MAIN_WINDOW_HANDLER_H
#define MAIN_WINDOW_HANDLER_H

#include "config.h"

#include "common.h"

#include <gtk/gtk.h>

enum MainWindow 
  {
    MW_MODE_SELECT = 100,
    MW_MODE_STATUS,
    MW_MODE_CONFIG,
    MW_CLEAR_STATUSBAR,
    MW_UPDATE_STATUSBAR,
    MW_REQUEST_MF,
    NO_CD_INDRIVE,
    CD_INDRIVE
  };


/* Creates main window. Returns the pointer to main frame which will be
 * used by select frame and update frames. ops can be one of WIDGET_CREATE,
 * MW_MODE_SELECT, MW_MODE_STATUS, MW_MODE_CONFIG, MW_UPDATE_STATUSBAR *
 *
 * main_data is used to install button callbacks */
GtkWidget *main_window_handler(int ops, char *status_bar_msg,
                               _main_data *main_data);

void mw_scan_button_clicked(GtkWidget *widget, gpointer callback_data);
void mw_cddb_button_clicked(GtkWidget *widget, gpointer callback_data);

#endif
