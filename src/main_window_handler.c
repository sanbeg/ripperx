#include "config.h"

#include <stdio.h>

#include <glib.h>
#include <glib/gi18n.h>

#include "interface_common.h"
#include "main.h"
#include "config_window_handler.h"
#include "players_manipulation.h"
#include "ripper_encoder_manipulation.h"
#include "select_frame_handler.h"
#include "job_control.h"
#include "status_frame_handler.h"
#include "cddb.h"
#include "misc_utils.h"
#include "main_window_handler.h"

/* Xpms */
#include "xpms/ripperX.xpm"
#include "xpms/config.xpm"
#include "xpms/scan.xpm"
#include "xpms/go.xpm"
#include "xpms/stop.xpm"
#include "xpms/cddb.xpm"
#include "xpms/exit.xpm"


void mw_config_button_clicked(GtkWidget *widget, gpointer callback_data);
void mw_stop_button_clicked(GtkWidget *widget, gpointer callback_data);
void mw_go_button_clicked(GtkWidget *widget, gpointer callback_data);
void mw_pause_button_clicked(GtkWidget *widget, gpointer callback_data);
void mw_exit_button_clicked(GtkWidget *widget, gpointer callback_data);
static int check_dirs();


void mw_config_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    main_window_handler(MW_MODE_CONFIG, 0, NULL);
    config_window_handler(WIDGET_CREATE, (_main_data *) callback_data);
    return;
}

void mw_scan_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    _main_data *main_data;

    main_data = (_main_data *) callback_data;
    memset(main_data, 0, sizeof(_main_data));

    select_frame_handler(WIDGET_DESTROY, 0, main_data);
    select_frame_handler(CLEAR_ENTRIES, 0, main_data);

    /* check to make sure there is a cd in the drive.. if not dim out
       cddb and go buttons to prevent errors from even happening....
       otherwise make sure they can use the buttons */

    if(!scan_cd(main_data))
    {
        main_window_handler(CD_INDRIVE, NULL, NULL);
    }
    else
    {
        main_window_handler(NO_CD_INDRIVE, NULL, NULL);
    }

    select_frame_handler(WIDGET_CREATE, 0, main_data);
    select_frame_handler(SF_SYNC_SELECT_FRAME, 0, main_data);
    return;
}

void mw_go_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    if(!check_dirs())
    {
        return;
    }

    play_cd_wav_mp3(STOP, CD, NULL);
    play_cd_wav_mp3(STOP, WAV, NULL);
    play_cd_wav_mp3(STOP, MP3, NULL);
    job_starter((_main_data *) callback_data);
    return;
}

void mw_stop_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    play_cd_wav_mp3(STOP, CD, NULL);
    play_cd_wav_mp3(STOP, WAV, NULL);
    play_cd_wav_mp3(STOP, MP3, NULL);
    return;
}

void mw_cddb_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    _main_data *main_data;

    main_data = (_main_data *) callback_data;
    cddb_main((_main_data *) callback_data);
    return;
}

void mw_exit_button_clicked(GtkWidget *widget, gpointer callback_data)
{
    int mode;

    mode = *(int *) callback_data;

    ripperX_exit(NULL, NULL);
    return;
}

GtkWidget *main_window_handler(int ops, char *statusbar_msg,
                               _main_data *main_data)
{
    static GtkWidget *main_frame, *statusbar;
    static int saved_mode;
    static int count = 0;

    static struct
    {
        GtkWidget *button;
        char **xpm;
      void (*func)(GtkWidget*, void*);
      //void *func;
        gpointer callback_data;
        int arrangement;
        /* If arrangement is TRUE, it will be packed using gtk_pack_start
         * function. Otherwise, it will use gtk_pack_end function */
        char *tooltip;
    } buttons[] =
    {
        {
            NULL, config_xpm, mw_config_button_clicked, NULL,
            TRUE, N_("Configuration")
        },
        {
            NULL, scan_xpm, mw_scan_button_clicked, NULL,
            TRUE, N_("Scan CD")
        },
        {
            NULL, stop_xpm, mw_stop_button_clicked, NULL,
            TRUE, N_("Stop playing")
        },
        {
            NULL, cddb_xpm, mw_cddb_button_clicked, NULL,
            TRUE, N_("Get track titles from CDDB server")
        },
        {
            NULL, go_xpm, mw_go_button_clicked, NULL,
            TRUE, N_("Start ripping&encoding")
        },
        {
            NULL, exit_xpm, mw_exit_button_clicked, NULL,
            FALSE, N_("Exit the program")
        },
    };

    buttons[ 0 ].callback_data = (gpointer) main_data;
    buttons[ 1 ].callback_data = (gpointer) main_data;
    buttons[ 3 ].callback_data = (gpointer) main_data;
    buttons[ 4 ].callback_data = (gpointer) main_data;
    buttons[ 5 ].callback_data = (gpointer) & saved_mode;

    switch(ops)
    {
        case WIDGET_CREATE :
        {
            /* Variables used only when creating widget */
            GtkWidget *main_window, *bbox, *vbox;
            GtkTooltips *tooltips;
            GtkWidget *pixmap;
            GdkPixmap *gdk_pixmap;
            GdkBitmap *mask;
            GtkStyle *style;
            GdkGC *gc;
            GdkColormap *colormap;
            static GdkColor tooltips_bg;
            int i, num_buttons;
            char welcome_msg_buf[ 100 ];

            num_buttons = sizeof(buttons) / sizeof(buttons[ 0 ]);

            main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            gtk_widget_set_name(main_window, "main window");
            gtk_widget_set_size_request(main_window, 508, 406);
            gtk_widget_realize(main_window);

            tooltips = gtk_tooltips_new();

            colormap = gdk_window_get_colormap(main_window->window);
            tooltips_bg.red = 61669;
            tooltips_bg.green = 59113;
            tooltips_bg.blue = 35979;
            gdk_color_alloc(colormap, &tooltips_bg);

            /*			gtk_tooltips_set_colors ( tooltips,
            			                          &tooltips_bg,
            			                          &main_window->style->fg[ GTK_STATE_NORMAL ] );*/
            g_object_set_data(G_OBJECT(main_window), "tooltips", tooltips);

            g_signal_connect(G_OBJECT(main_window), "destroy",
                             G_CALLBACK(ripperX_exit),
                             NULL);
            g_signal_connect(G_OBJECT(main_window), "delete_event",
                             G_CALLBACK(ripperX_exit),
                             NULL);

            /* Create a vbox to contain things */
            vbox = gtk_vbox_new(FALSE, 0);
            gtk_container_add(GTK_CONTAINER(main_window), vbox);
            gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
            gtk_widget_realize(vbox);
            gtk_widget_show(vbox);

            /* Create button box & buttons */
            bbox = gtk_hbox_new(FALSE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 0);
            gtk_container_set_border_width(GTK_CONTAINER(bbox), 0);

            gtk_widget_realize(bbox);
            gtk_widget_show(bbox);

            /* logo */
            style = gtk_widget_get_default_style();
            gc = style->black_gc;
            gdk_pixmap = gdk_pixmap_create_from_xpm_d(bbox->window, &mask,
                         &style->bg[ GTK_STATE_NORMAL ],
                         ripperX_xpm);
            pixmap = gtk_pixmap_new(gdk_pixmap, mask);
            gtk_box_pack_start(GTK_BOX(bbox), pixmap, FALSE, FALSE, 0);

            for(i = 0; i < num_buttons; i++)
            {
                /* Create pixmap */
                style = gtk_widget_get_default_style();
                gc = style->black_gc;
                gdk_pixmap = gdk_pixmap_create_from_xpm_d(bbox->window, &mask,
                             &style->bg[ GTK_STATE_NORMAL ],
                             buttons[ i ].xpm);
                pixmap = gtk_pixmap_new(gdk_pixmap, mask);

                /* Pack pixmap into button */
                buttons[ i ].button = gtk_button_new();
                gtk_container_add(GTK_CONTAINER(buttons[ i ].button), pixmap);

                g_signal_connect(G_OBJECT(buttons[ i ].button), "clicked",
                                 G_CALLBACK(buttons[ i ].func),
                                 (gpointer) buttons[ i ].callback_data);

                if(buttons[ i ].arrangement == TRUE)
                {
                    gtk_box_pack_start(GTK_BOX(bbox), buttons[ i ].button, FALSE, FALSE, 0);
                }
                else
                {
                    gtk_box_pack_end(GTK_BOX(bbox), buttons[ i ].button, FALSE, FALSE, 0);
                }

                gtk_tooltips_set_tip(tooltips, buttons[ i ].button,
                                     buttons[ i ].tooltip, NULL);
            }

            /* Create main frame */
            main_frame = gtk_frame_new("ripperX");
            gtk_box_pack_start(GTK_BOX(vbox), main_frame, TRUE, TRUE, 0);

            statusbar = gtk_statusbar_new();
            gtk_box_pack_end(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

            /* Push welcome message */
            sprintf(welcome_msg_buf, " RipperX, v%s", VERSION);
            gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1, welcome_msg_buf);

            /* Read main_data & Create select window here */

            count = scan_cd(main_data);

            select_frame_handler(WIDGET_CREATE, 0, main_data);
            select_frame_handler(SF_SYNC_SELECT_FRAME, 0, main_data);
            gtk_widget_show_all(main_window);

            /* Do a CDDB lookup for this CD only if cd is actually is drive */
            if(config.cddb_config.auto_lookup && !count)
            {
                cddb_main(main_data);
            }

        }
        /* go on and set mode to select mode */

        case MW_MODE_SELECT :
            saved_mode = ops;
            /* where now is used by ripperX_exit */
            where_now = SELECT_FRAME;
            gtk_widget_set_sensitive(buttons[ 0 ].button, TRUE);
            gtk_widget_set_sensitive(buttons[ 1 ].button, TRUE);
            gtk_widget_set_sensitive(buttons[ 2 ].button, TRUE);

            /* disable cddb and go buttons since no cd anyway... prevent
               stupid errors*/
            if(count)
            {
                gtk_widget_set_sensitive(buttons[ 3 ].button, FALSE);
                gtk_widget_set_sensitive(buttons[ 4 ].button, FALSE);
            }
            else
            {
                gtk_widget_set_sensitive(buttons[ 3 ].button, TRUE);
                gtk_widget_set_sensitive(buttons[ 4 ].button, TRUE);
            }

            gtk_widget_set_sensitive(buttons[ 5 ].button, TRUE);

            return main_frame;

        case MW_MODE_STATUS :
            saved_mode = ops;
            /* where now is used by ripperX_exit */
            where_now = STATUS_FRAME;
            gtk_widget_set_sensitive(buttons[ 0 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 1 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 2 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 3 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 4 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 5 ].button, TRUE);

            return main_frame;

        case MW_MODE_CONFIG :
            saved_mode = ops;
            /* where now is used by ripperX_exit */
            where_now = CONFIG_WINDOW;
            gtk_widget_set_sensitive(buttons[ 0 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 1 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 2 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 3 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 4 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 5 ].button, TRUE);

            return main_frame;

        case MW_CLEAR_STATUSBAR :

            while(--count >= 0)
            {
                gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 1);
            }

            count++;
            return main_frame;

        case MW_UPDATE_STATUSBAR :
            count++;
            gtk_statusbar_push(GTK_STATUSBAR(statusbar), 1,
                               statusbar_msg);
            return main_frame;

        case MW_REQUEST_MF :
            return main_frame;

        case NO_CD_INDRIVE:
            gtk_widget_set_sensitive(buttons[ 3 ].button, FALSE);
            gtk_widget_set_sensitive(buttons[ 4 ].button, FALSE);
            return main_frame;

        case CD_INDRIVE:
            gtk_widget_set_sensitive(buttons[ 3 ].button, TRUE);
            gtk_widget_set_sensitive(buttons[ 4 ].button, TRUE);
            return main_frame;

    }

    return main_frame;
}

static int check_dirs()
{
    long wav_free, mp3_free;
    int rc;

    rc = check_dir(config.wav_path);

    switch(rc)
    {
        case MISC_DOES_NOT_EXISTS:
            rc = dialog_handler(WIDGET_CREATE, FALSE, 7, 0, NULL, NULL, 0);

            if(!rc)
            {
                return 0;
            }
            else
            {
                if(create_dir(config.wav_path) != 0)
                {
                    err_handler(29, NULL);
                    return 0;
                }
            }

            break;
        case MISC_NOT_DIR:
        case MISC_NOT_WRITABLE:
            err_handler(27, NULL);
            return 0;
            break;
    }

    rc = check_dir(config.mp3_path);

    switch(rc)
    {
        case MISC_DOES_NOT_EXISTS:
            rc = dialog_handler(WIDGET_CREATE, FALSE, 8, 0, NULL, NULL, 0);

            if(!rc)
            {
                return 0;
            }
            else
            {
                if(create_dir(config.mp3_path) != 0)
                {
                    err_handler(30, NULL);
                    return 0;
                }
            }

            break;
        case MISC_NOT_DIR:
        case MISC_NOT_WRITABLE:
            err_handler(28, NULL);
            return 0;
            break;
    }

    wav_free = check_free_space(config.wav_path);
    mp3_free = check_free_space(config.mp3_path);

    if(wav_free < 500 * 1024)
    {
        /* warn if less than 500 MB free on wav partition */
        rc = dialog_handler(WIDGET_CREATE, FALSE, 5, 0, NULL, NULL, 0);

        if(!rc)
        {
            return 0;
        }
    }

    if(wav_free != mp3_free && mp3_free < 100 * 1024)
    {
        /* only warn if on different partition and less
         * than 100 MB free */
        rc = dialog_handler(WIDGET_CREATE, FALSE, 6, 0, NULL, NULL, 0);

        if(!rc)
        {
            return 0;
        }
    }

    /* good to go */
    return 1;

}

