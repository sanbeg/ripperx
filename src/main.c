#include "config.h"

#ifdef ENABLE_NLS
#include <glib/gi18n.h>
#else
#define _(a) (a)
#define N_(a) (a)
#endif

#include "interface_common.h"
#include "main_window_handler.h"
#include "err_dialog_handler.h"
#include "job_control.h"
#include "config_rw.h"

#include "main.h"

void ripperX_init(_main_data *main_data);

/* Global variable */
_config config;
int where_now;


void ripperX_init(_main_data *main_data)
{
    memset(main_data, 0, sizeof(_main_data));
    read_config();
    main_window_handler(WIDGET_CREATE, 0, main_data);
    return;
}


void ripperX_exit(GtkWidget *widget, gpointer callback_data)
{
    if(where_now == STATUS_FRAME)
    {
        job_controller(JC_PAUSE, NULL);

        /* Confirm */
        if(dialog_handler(WIDGET_CREATE, FALSE, DL_ABORT_CONFIRM,
                          FALSE, NULL, NULL, 0) == FALSE)
        {
            job_controller(JC_CONT, NULL);
            return;
        }

        /* Terminate current job */
        if(dialog_handler(WIDGET_CREATE, FALSE, DL_DELETE_ON_ABORT,
                          FALSE, NULL, NULL, 0) == TRUE)
        {
            job_controller(JC_ABORT_ALL_DELETE, NULL);
        }
        else
        {
            job_controller(JC_ABORT_ALL, NULL);
        }
    }

    main_window_handler(WIDGET_DESTROY, NULL, NULL);
    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    _main_data main_data;
    gtk_set_locale();
    gtk_init(&argc, &argv);
    setlocale(LC_NUMERIC, "POSIX");
#ifdef ENABLE_NLS
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);
#endif /*ENABLE_NLS*/
    ripperX_init(&main_data);
    gtk_main();
    return 0;
}
