
#include <glib.h>
#include <glib/gi18n.h>
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
			struct stat st;
			char check_dir[ MAX_FILE_PATH_LENGTH + 1];
			char *checked_dir;
			saved_cur_dir = cur_dir;
			checked_dir = check_dir;
			strncpy(check_dir, cur_dir, MAX_FILE_PATH_LENGTH );
			
			/* Add a final / to input directory string if missing and input string is only a
			directory (i.e. no filename); but pass through any other strings.  This allows
			GtkFileSelection widget to show proper directory level upon entering. */
			if ( ( lstat( checked_dir, &st ) >= 0 ) && ( S_ISDIR( st.st_mode ) )) {
				if( ( check_dir[ strlen( checked_dir ) - 1 ] != '/' )
						&& ( strlen(checked_dir) > 0 )
						&& ( strlen(checked_dir) < MAX_FILE_PATH_LENGTH ) ) {
					checked_dir = strcat( check_dir, "/" );
				}
			}

			filew = gtk_file_selection_new(_("Select a directory"));
			gtk_window_set_position ( GTK_WINDOW ( filew ), GTK_WIN_POS_MOUSE );
			gtk_file_selection_show_fileop_buttons ( GTK_FILE_SELECTION ( filew ) );

			id = g_signal_connect( G_OBJECT( filew ), "destroy",
			                         G_CALLBACK( dw_cancel_clicked ),
			                         NULL );
			g_signal_connect( G_OBJECT( GTK_FILE_SELECTION( filew ) ->ok_button ),
			                    "clicked",
			                    G_CALLBACK( dw_ok_clicked ), NULL );
			g_signal_connect( G_OBJECT( GTK_FILE_SELECTION( filew ) ->cancel_button ),
			                    "clicked",
			                    G_CALLBACK( dw_cancel_clicked ),
			                    NULL );
			gtk_file_selection_set_filename( GTK_FILE_SELECTION( filew ), checked_dir );
			gtk_widget_hide( GTK_WIDGET( GTK_FILE_SELECTION( filew ) ->fileop_del_file ));
			gtk_widget_hide( GTK_WIDGET( GTK_FILE_SELECTION( filew ) ->fileop_ren_file ));
			gtk_widget_set_sensitive( GTK_WIDGET( GTK_FILE_SELECTION( filew ) ->file_list ),
			                    FALSE );
			gtk_widget_show( filew );
			gtk_main();
			g_signal_handler_disconnect( G_OBJECT( filew ), id );
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
			if ( lstat( temp, &st ) < 0 ) {
				err_handler( INVALID_FILE_SELECTION_ERR, NULL );
				return NULL;
			}
			if ( !S_ISDIR( st.st_mode ) ) {
				gtk_file_selection_set_filename( GTK_FILE_SELECTION( filew ),
				                                 file_path_without_name( temp ) );
				temp = gtk_file_selection_get_filename( GTK_FILE_SELECTION( filew ) );
			}
			/* remove final directory ../ from directory string if present */
			if( strlen( temp ) > 3 ) {
				if( ( temp[ strlen( temp ) - 1 ] == '/' ) &&
						( temp[ strlen( temp ) - 2 ] == '.' ) &&
						( temp[ strlen( temp ) - 3 ] == '.' ) &&
						( temp[ strlen( temp ) - 4 ] == '/' ) ) {
					temp[ strlen( temp ) - 3 ] = '\0';
					gtk_file_selection_set_filename( GTK_FILE_SELECTION( filew ),
						temp );
				}
			}
			/* remove final directory ./ from directory string if present */
			if ( strlen( temp ) > 2 ) {
				if( ( temp[ strlen( temp ) - 1 ] == '/' ) &&
						( temp[ strlen( temp ) - 2 ] == '.' ) &&
						( temp[ strlen( temp ) - 3 ] == '/' ) ) {
					temp[ strlen( temp ) - 2 ] = '\0';
					gtk_file_selection_set_filename( GTK_FILE_SELECTION( filew ),
						temp );
				}
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


