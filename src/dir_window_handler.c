
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>

#include "misc_utils.h"
#include "interface_common.h"

#include "dir_window_handler.h"

void dw_cancel_clicked( GtkWidget *widget, gpointer callback_data );
void dw_ok_clicked( GtkWidget *widget, gpointer callback_data );


void dw_cancel_clicked( GtkWidget *widget, gpointer callback_data )
{
	dir_window_handler( DW_CANCEL, NULL );
}

void dw_ok_clicked( GtkWidget *widget, gpointer callback_data )
{
	dir_window_handler( DW_OK, NULL );
}

char *dir_window_handler( int ops, char *cur_dir )
{
	static GtkWidget * filew;
	static int id;
	static char buf[ MAX_FILE_PATH_LENGTH ];
	static char *saved_cur_dir;

	switch ( ops ) {
	case WIDGET_CREATE : {
			saved_cur_dir = cur_dir;

			filew = gtk_file_selection_new( "Select a directory" );
			gtk_window_set_position ( GTK_WINDOW ( filew ), GTK_WIN_POS_MOUSE );
			gtk_file_selection_show_fileop_buttons ( GTK_FILE_SELECTION ( filew ) );

			id = gtk_signal_connect( GTK_OBJECT( filew ), "destroy",
			                         GTK_SIGNAL_FUNC( dw_cancel_clicked ),
			                         NULL );
			gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( filew ) ->ok_button ),
			                    "clicked",
			                    GTK_SIGNAL_FUNC( dw_ok_clicked ), NULL );
			gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( filew ) ->cancel_button ),
			                    "clicked",
			                    GTK_SIGNAL_FUNC( dw_cancel_clicked ),
			                    NULL );
			gtk_file_selection_set_filename( GTK_FILE_SELECTION( filew ), cur_dir );

			gtk_widget_show( filew );
			gtk_main();
			gtk_signal_disconnect( GTK_OBJECT( filew ), id );
			strncpy( buf,
			         gtk_file_selection_get_filename( GTK_FILE_SELECTION( filew ) ),
			         sizeof( buf ) );
			gtk_widget_destroy( filew );
			return buf;
		}

	case DW_OK : {
			struct stat st;
			char *temp;

			temp = gtk_file_selection_get_filename( GTK_FILE_SELECTION( filew ) );
			if ( lstat( temp, &st )
				        < 0 ) {
				err_handler( INVALID_FILE_SELECTION_ERR, NULL );
				return NULL;
			}
			if ( !S_ISDIR( st.st_mode ) ) {
				gtk_file_selection_set_filename( GTK_FILE_SELECTION( filew ),
				                                 file_path_without_name( temp ) );
			}
			gtk_main_quit();
			return NULL;
		}

	case DW_CANCEL :
		gtk_file_selection_set_filename( GTK_FILE_SELECTION( filew ), saved_cur_dir );
		gtk_main_quit();
		return NULL;
	}
	return NULL;
}


