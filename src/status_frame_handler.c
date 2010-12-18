#include "config.h"

#include <glib.h>
#include <glib/gi18n.h>

#include "misc_utils.h"
#include "job_control.h"
#include "interface_common.h"
#include "main_window_handler.h"
#include "status_frame_handler.h"

#include "xpms/ripperX_box.xpm"


#define WMS_P_C_BUTTON_ACT          100
#define WMS_ABORT_BUTTON_ACT        101
#define WMS_ABORT_ALL_BUTTON_ACT    102

#define WMS_TIME_UPDATE_CYCLE       1

void time_status_frame_handler(int ops, GtkWidget *time_frame, _stat *stat);

/* Callback data is a pinter to the int type */
void wms_p_c_button_clicked(GtkWidget *widget, gpointer callback_data);
void wms_abort_button_clicked(GtkWidget *widget, gpointer callback_data);
void wms_abort_all_button_clicked(GtkWidget *widget, gpointer callback_data);


void time_status_frame_handler(int ops, GtkWidget *time_frame, _stat *stat)
{
    static GtkWidget *table = NULL;
    static GtkWidget *total_time_elapsed_label, *total_time_remain_label;
    static GtkWidget *tracks_done_label, *tracks_remain_label;
    char buf[ 100 ];

    switch(ops)
    {
        case WIDGET_CREATE :
        {
            GtkWidget *label;

            if(table != NULL)
            {
                return;
            }

            table = gtk_table_new(4, 2, TRUE);

            gtk_container_set_border_width(GTK_CONTAINER(table), 5);
            gtk_container_add(GTK_CONTAINER(time_frame), table);

            label = gtk_label_new(_("Total time elapsed: "));
            gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
            total_time_elapsed_label = gtk_label_new(time_to_readable(0));
            gtk_label_set_justify(GTK_LABEL(total_time_elapsed_label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), total_time_elapsed_label, 1, 2, 0, 1,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

            label = gtk_label_new(_("Total time remaining: "));
            gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
            total_time_remain_label = gtk_label_new(time_to_readable(0));
            gtk_label_set_justify(GTK_LABEL(total_time_remain_label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), total_time_remain_label, 1, 2, 1, 2,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

            label = gtk_label_new(_("Tracks completed: "));
            gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
            tracks_done_label = gtk_label_new("0");
            gtk_label_set_justify(GTK_LABEL(tracks_done_label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), tracks_done_label, 1, 2, 2, 3,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

            label = gtk_label_new(_("Tracks remaining: "));
            gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
            tracks_remain_label = gtk_label_new("0");
            gtk_label_set_justify(GTK_LABEL(tracks_remain_label), GTK_JUSTIFY_LEFT);
            gtk_table_attach(GTK_TABLE(table), tracks_remain_label, 1, 2, 3, 4,
                             GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

            gtk_widget_show_all(table);
            return;
        }

        case WIDGET_UPDATE :

            gtk_label_set_text(GTK_LABEL(total_time_elapsed_label), time_to_readable(stat->total_time_elapsed));
            gtk_label_set_text(GTK_LABEL(total_time_remain_label), time_to_readable(stat->total_time_remain));
            snprintf(buf, sizeof(buf), "%d", stat->tracks_done);
            gtk_label_set_text(GTK_LABEL(tracks_done_label), buf);
            snprintf(buf, sizeof(buf), "%d", stat->tracks_remain);
            gtk_label_set_text(GTK_LABEL(tracks_remain_label), buf);

            return;

        case WIDGET_DESTROY :

            if(table == NULL)
            {
                return;
            }

            gtk_widget_destroy(table);
            table = NULL;
            return;
    }
}

/* Callback data is pointer to type */

void wms_p_c_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    wm_status_frame_handler(WMS_P_C_BUTTON_ACT, *(int *) callback_data,
                            NULL, NULL);
}

void wms_abort_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    wm_status_frame_handler(WMS_ABORT_BUTTON_ACT, *(int *) callback_data,
                            NULL, NULL);
}

void wm_status_frame_handler(int ops, int type,
                             _stat *stat, char *graph_string)
{
    static GtkWidget *vbox = NULL, *wav_pbar, *mp3_pbar, *total_pbar;
    static GtkWidget *time_frame, *wav_plabel, *mp3_plabel, *total_plabel;
    static GtkWidget *wav_tlabel, *mp3_tlabel;
    static GtkWidget *hbox;
    static GtkWidget *p_c_button, *pause_label, *cont_label;
    static GtkWidget *pixmap = NULL;
    static char percentage_buf[ 10 ];
    static char graph_string_buf[ 50 ];
    static int count;
    static GdkPixmap *gdk_pixmap;
    static GdkBitmap *mask;
    char buf[ MAX_FILE_NAME_LENGTH + 100 ];

    switch(ops)
    {
        case WIDGET_CREATE :
        {
            GtkWidget *main_frame, *label, *button, *temp_hbox, *separator;
            GtkStyle *style;
            GdkGC *gc;

            if(vbox != NULL)
            {
                return;
            }

            count = WMS_TIME_UPDATE_CYCLE - 1;
            main_frame = main_window_handler(MW_REQUEST_MF, 0, NULL);

            vbox = gtk_vbox_new(FALSE, 5);
            gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
            gtk_container_add(GTK_CONTAINER(main_frame), vbox);
            gtk_widget_realize(vbox);

            /* Create pixmap */
            if(pixmap == NULL)
            {
                style = gtk_widget_get_default_style();
                gc = style->black_gc;
                gdk_pixmap = gdk_pixmap_create_from_xpm_d(vbox->window, &mask,
                             &style->bg[ GTK_STATE_NORMAL ],
                             ripperX_box_xpm);
            }

            /* ripping progress */
            hbox = gtk_hbox_new(FALSE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 0);
            sprintf(buf, _("Ripping track %d"), stat->wav_track + 1);
            wav_tlabel = gtk_label_new(buf);
            gtk_label_set_justify(GTK_LABEL(wav_tlabel), GTK_JUSTIFY_LEFT);
            gtk_widget_set_size_request(wav_tlabel, 130, -1);
            gtk_box_pack_start(GTK_BOX(hbox), wav_tlabel, FALSE, FALSE, 0);

            wav_pbar = gtk_progress_bar_new();
            gtk_widget_set_size_request(wav_pbar, 250, 22);
            gtk_box_pack_start(GTK_BOX(hbox), wav_pbar, FALSE, FALSE, 0);
            wav_plabel = gtk_label_new(" 0%");
            gtk_box_pack_start(GTK_BOX(hbox), wav_plabel, FALSE, FALSE, 0);

            /* encoding progress */
            hbox = gtk_hbox_new(FALSE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 0);
            sprintf(buf, _("Encoding track %d"), stat->wav_track + 1);
            mp3_tlabel = gtk_label_new(buf);
            gtk_label_set_justify(GTK_LABEL(mp3_tlabel), GTK_JUSTIFY_LEFT);
            gtk_widget_set_size_request(mp3_tlabel, 130, -1);
            gtk_box_pack_start(GTK_BOX(hbox), mp3_tlabel, FALSE, FALSE, 0);

            mp3_pbar = gtk_progress_bar_new();
            gtk_widget_set_size_request(mp3_pbar, 250, 22);
            gtk_box_pack_start(GTK_BOX(hbox), mp3_pbar, FALSE, FALSE, 0);
            mp3_plabel = gtk_label_new(" 0%");
            gtk_box_pack_start(GTK_BOX(hbox), mp3_plabel, FALSE, FALSE, 0);

            /* total progress */
            hbox = gtk_hbox_new(FALSE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 0);
            label = gtk_label_new(_("Total progress"));
            gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
            gtk_widget_set_size_request(label, 130, -1);
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

            total_pbar = gtk_progress_bar_new();
            gtk_widget_set_size_request(total_pbar, 250, 22);
            gtk_box_pack_start(GTK_BOX(hbox), total_pbar, FALSE, FALSE, 0);
            total_plabel = gtk_label_new(" 0%");
            gtk_box_pack_start(GTK_BOX(hbox), total_plabel, FALSE, FALSE, 0);


            /* Row with time frame & pixmap */
            temp_hbox = gtk_hbox_new(FALSE, 10);
            gtk_box_pack_start(GTK_BOX(vbox), temp_hbox, TRUE, TRUE, 0);

            time_frame = gtk_frame_new(_("Time info"));
            gtk_widget_set_size_request(time_frame, 275, 0);
            gtk_box_pack_start(GTK_BOX(temp_hbox), time_frame, FALSE, FALSE, 0);

            time_status_frame_handler(WIDGET_CREATE, time_frame, stat);

            /* Pixmap */
            pixmap = gtk_pixmap_new(gdk_pixmap, mask);
            gtk_box_pack_start(GTK_BOX(temp_hbox), pixmap, TRUE, TRUE, 0);

            /* Separator */
            separator = gtk_hseparator_new();
            gtk_widget_set_size_request(separator, 0, 10);
            gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 0);

            /* Buttons row */
            temp_hbox = gtk_hbox_new(FALSE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), temp_hbox, TRUE, TRUE, 0);

            /* Abort button */
            button = gtk_button_new_with_label(_("Abort"));
            GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
            gtk_widget_set_size_request(button, 80, -1);
            g_signal_connect(G_OBJECT(button), "clicked",
                             G_CALLBACK(wms_abort_button_clicked),
                             &type);
            gtk_box_pack_end(GTK_BOX(temp_hbox), button, FALSE, FALSE, 0);

            /* Pause continue button */
            p_c_button = gtk_button_new();
            GTK_WIDGET_SET_FLAGS(p_c_button, GTK_CAN_DEFAULT);
            gtk_widget_set_size_request(p_c_button, 80, 0);
            pause_label = gtk_label_new(_("Pause"));
            gtk_container_add(GTK_CONTAINER(p_c_button), pause_label);
            g_signal_connect(G_OBJECT(p_c_button), "clicked",
                             G_CALLBACK(wms_p_c_button_clicked),
                             &type);
            gtk_box_pack_end(GTK_BOX(temp_hbox), p_c_button, FALSE, FALSE, 0);
            gtk_widget_grab_default(p_c_button);

            cont_label = NULL;

            gtk_widget_show_all(vbox);
            return;
        }

        case WIDGET_UPDATE :

            if(count++ < WMS_TIME_UPDATE_CYCLE)
            {
                return;
            }

            if(stat->ripping)
            {
                gtk_widget_set_sensitive(wav_tlabel, TRUE);
                gtk_widget_set_sensitive(wav_pbar, TRUE);
                gtk_widget_set_sensitive(wav_plabel, TRUE);
                sprintf(buf, _("Ripping track %d"), stat->wav_track + 1);
                gtk_label_set_text(GTK_LABEL(wav_tlabel), buf);
                gtk_progress_bar_update(GTK_PROGRESS_BAR(wav_pbar), stat->wav_progress);

                snprintf(percentage_buf, sizeof(percentage_buf),
                         "%2d%%", (int)(stat->wav_progress * 100));
                gtk_label_set_text(GTK_LABEL(wav_plabel), percentage_buf);
            }
            else
            {
                gtk_widget_set_sensitive(wav_tlabel, FALSE);
                gtk_widget_set_sensitive(wav_pbar, FALSE);
                gtk_widget_set_sensitive(wav_plabel, FALSE);
                gtk_label_set_text(GTK_LABEL(wav_tlabel), _("Not ripping"));
                gtk_progress_bar_update(GTK_PROGRESS_BAR(wav_pbar), 0);
                gtk_label_set_text(GTK_LABEL(wav_plabel), "-");
            }

            if(stat->encoding)
            {
                gtk_widget_set_sensitive(mp3_tlabel, TRUE);
                gtk_widget_set_sensitive(mp3_pbar, TRUE);
                gtk_widget_set_sensitive(mp3_plabel, TRUE);
                sprintf(buf, _("Encoding track %d"), stat->mp3_track + 1);
                gtk_label_set_text(GTK_LABEL(mp3_tlabel), buf);
                gtk_progress_bar_update(GTK_PROGRESS_BAR(mp3_pbar), stat->mp3_progress);

                snprintf(percentage_buf, sizeof(percentage_buf),
                         "%2d%%", (int)(stat->mp3_progress * 100));
                gtk_label_set_text(GTK_LABEL(mp3_plabel), percentage_buf);
            }
            else
            {
                gtk_widget_set_sensitive(mp3_tlabel, FALSE);
                gtk_widget_set_sensitive(mp3_pbar, FALSE);
                gtk_widget_set_sensitive(mp3_plabel, FALSE);
                gtk_label_set_text(GTK_LABEL(mp3_tlabel), _("Not encoding"));
                gtk_progress_bar_update(GTK_PROGRESS_BAR(mp3_pbar), 0);
                gtk_label_set_text(GTK_LABEL(mp3_plabel), "-");
            }

            /* 2002/03/15 - <tmancill@debian.org> - hack to ensure that
              stat->total_progress is always between 0 and 1 */

            gtk_progress_bar_update(GTK_PROGRESS_BAR(total_pbar),
                                    (stat->total_progress < 0.0) ? 0.0
                                    : (stat->total_progress > 1.0) ? 1.0
                                    : stat->total_progress);

            snprintf(percentage_buf, sizeof(percentage_buf), "%2d%%",
                     (int)(((stat->total_progress < 0.0) ? 0.0
                            : (stat->total_progress > 1.0) ? 1.0
                            : stat->total_progress) * 100));

            gtk_label_set_text(GTK_LABEL(total_plabel), percentage_buf);

            if(graph_string != NULL)
            {
                snprintf(graph_string_buf, sizeof(graph_string_buf),
                         "Cdparanoia: %s", graph_string);
                main_window_handler(MW_UPDATE_STATUSBAR, graph_string_buf, NULL);
            }

            if(!stat->ripping)
            {
                main_window_handler(MW_UPDATE_STATUSBAR, "", NULL);
            }


            time_status_frame_handler(WIDGET_UPDATE, time_frame, stat);
            count = 0;

            return;

        case WMS_P_C_BUTTON_ACT :

            if(pause_label != NULL)
            {
                job_controller(JC_PAUSE, NULL);

                gtk_widget_destroy(pause_label);
                pause_label = NULL;
                cont_label = gtk_label_new(_("Continue"));
                gtk_container_add(GTK_CONTAINER(p_c_button), cont_label);
                gtk_widget_show(cont_label);
            }
            else
            {
                job_controller(JC_CONT, NULL);

                gtk_widget_destroy(cont_label);
                cont_label = NULL;
                pause_label = gtk_label_new(_("Pause"));
                gtk_container_add(GTK_CONTAINER(p_c_button), pause_label);
                gtk_widget_show(pause_label);
            }

            return;

        case WMS_ABORT_BUTTON_ACT :
        case WMS_ABORT_ALL_BUTTON_ACT :

            job_controller(JC_PAUSE, NULL);

            if(dialog_handler(WIDGET_CREATE, FALSE, DL_ABORT_CONFIRM,
                              FALSE, NULL, NULL, 0) == FALSE)
            {
                job_controller(JC_CONT, NULL);
                return;
            }

            if(dialog_handler(WIDGET_CREATE, FALSE, DL_DELETE_ON_ABORT,
                              FALSE, NULL, NULL, 0) == TRUE)
            {
                if(ops == WMS_ABORT_BUTTON_ACT)
                {
                    job_controller(JC_ABORT_DELETE, NULL);
                }

                else
                {
                    job_controller(JC_ABORT_ALL_DELETE, NULL);
                }
            }
            else
            {
                if(ops == WMS_ABORT_BUTTON_ACT)
                {
                    job_controller(JC_ABORT, NULL);
                }
                else
                {
                    job_controller(JC_ABORT_ALL, NULL);
                }
            }

            return;

        case WIDGET_DESTROY :
            main_window_handler(MW_CLEAR_STATUSBAR, NULL , NULL);
            time_status_frame_handler(WIDGET_DESTROY, NULL, NULL);

            if(vbox == NULL)
            {
                return;
            }

            gtk_widget_destroy(vbox);
            vbox = NULL;
            return;
    }
}
