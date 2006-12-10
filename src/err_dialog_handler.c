/* file err_dialog_handler.c
 * 
 * defines procedures for error messages or dialogs
 *
 * Ralf Engels  10/06/1999  changed errno stuff 
 *                          (strange who does such things)
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>

#include "interface_common.h"

#include "err_dialog_handler.h"

void dl_callback( GtkWidget *widget, gpointer callback_data );
/* When with_entry is FALSE and entry_default is not NULL, entry_default
 * will be displayed as extra msg */


const char *err_msg[] = {
	"An error has occured while forking",
	"Cannot create pipe",
	"Cannot open /dev/null",
	"Memory allcation error has occurred",
	"An error has occurred while trying to\nreopen a file as a stream (fdopen)",
	"Cannot read from the pipe",
	"Cannot delete file",
	"Cannot open configuration file\nloading default values",
	"Cannot create configuration file ~/.ripperXrc",
	"Cannot open a pty\nMaybe all ptys are busy",
	"/////  use perror msg until here /////",
	"Too many arguments",
	"Job already in progress",
	"Please select a track to rip or encode. \nIf you did select tracks, they may already have been ripped.",
	"Make sure an audio disc is in the drive and that you are \neither running ripperX as root, are a member\nof the \"cdrom\" group, or otherwise have appropriate\npermissions to access the CDROM device.\n",
	"The file name is too long",
	"Some entry(s) are empty",
	"An error has occurred while parsing ~/.ripperXrc",
	"Some field(s) are empty. Subsituting default value",
	"Invalid selection",
	"Could not connect to CDDB server",
	"Connection refused from CDDB server",
	"CDDB Server Error",
	"CD Not Found in CD Database",
	"Cannot parse the format string",
	"Plugin not present",
	"Error while locking file.\nMake sure the WAV dir and MP3 dir paths are correct and \nyou have permission to write to those directories.",
	"The WAV dir is not writable.",
	"The MP3 dir is not writable.",
	"Cannot create WAV dir",
	"Cannot create MP3 dir",
	"Make sure an audio disc is in the CDROM device.",
	"Make sure that you are a member of the the \"cdrom\" group,\nor otherwise have appropriate permissions to access the CDROM device.",
};

const struct {
	char *title;
	char *msg;
} dialog_data[] = {
	{ "Abort confirmation",
		"Do you really want to abort?" },
	{ "Wanna delete?",
		"Do you want to delete current file?" },
	{ "Wanna overwrite?",
		"The file already exists. Do you want to overwrite?" },
	{ "Enter file name",
		"Enter new file name" },
	{ "Wanna create config file?" ,
		"Config file ~/.ripperXrc does not exist.\nDo you want to create config file?" },
	{ "Low diskspace warning",
		"There is less than 500 megabytes free on the wav partition.\n Do you want to continue?" },
	{  "Low diskspace warning",
		"There is less than 100 megabytes free on the mp3 partition.\n Do you want to continue?" },
	{  "Directory does not exist",
		"The WAV dir does not exist. Create it?" },
	{  "Directory does not exist",
		"The MP3 dir does not exist. Create it?" }
};

const char * status_msg [] = {
	"Finished ripping",
	"Finished encoding"
};

void err_handler( int err_code, const char *extra_msg )
{
	GtkWidget * window, *vbox, *hbox, *label, *separator, *button;
	char buf[ 20 ];

	window = gtk_window_new( GTK_WINDOW_DIALOG );
	gtk_window_set_position( GTK_WINDOW( window ), GTK_WIN_POS_CENTER );
	gtk_window_set_title( GTK_WINDOW( window ), "Error" );
	gtk_signal_connect( GTK_OBJECT( window ), "destroy",
			GTK_SIGNAL_FUNC( gtk_widget_destroyed ),
			NULL );
	gtk_container_border_width( GTK_CONTAINER( window ), 5 );

	vbox = gtk_vbox_new( FALSE, 3 );
	gtk_container_border_width( GTK_CONTAINER( vbox ), 0 );
	gtk_container_add( GTK_CONTAINER( window ), vbox );

	sprintf( buf, "Error Code %d", err_code );
	label = gtk_label_new( buf );
	gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
	gtk_box_pack_start( GTK_BOX( vbox ), label, TRUE, FALSE, 0 );

	label = gtk_label_new( err_msg[ err_code ] );
	gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
	gtk_box_pack_start( GTK_BOX( vbox ), label, TRUE, FALSE, 0 );

	if ( err_code < END_PERROR ) {
		hbox = gtk_hbox_new( FALSE, 3 );
		gtk_box_pack_start( GTK_BOX( vbox ), hbox, TRUE, TRUE, 0 );

		label = gtk_label_new( "Perror: " );
		gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
		gtk_box_pack_start( GTK_BOX( hbox ), label, TRUE, TRUE, 0 );

		label = gtk_label_new( strerror( errno ) );
		gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
		gtk_box_pack_start( GTK_BOX( hbox ), label, TRUE, TRUE, 0 );
	}

	if ( extra_msg != NULL ) {
		label = gtk_label_new( extra_msg );
		gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
		gtk_label_set_line_wrap( GTK_LABEL( label ), TRUE );
		gtk_box_pack_start( GTK_BOX( vbox ), label, TRUE, FALSE, 0 );
	}

	separator = gtk_hseparator_new();
	gtk_box_pack_start( GTK_BOX( vbox ), separator, FALSE, FALSE, 0 );

	button = gtk_button_new_with_label( "Ok" );
	GTK_WIDGET_SET_FLAGS( button, GTK_CAN_DEFAULT );
	gtk_signal_connect_object( GTK_OBJECT( button ), "clicked",
			GTK_SIGNAL_FUNC( gtk_widget_destroy ),
			GTK_OBJECT( window ) );
	gtk_box_pack_start( GTK_BOX( vbox ), button, TRUE, FALSE, 0 );
	gtk_widget_grab_default( button );

	gtk_widget_show_all( window );
}

void dl_callback( GtkWidget *widget, gpointer callback_data )
{
	dialog_handler( DL_OK_PRESSED, TRUE, 0, TRUE, NULL, NULL, 0 );
}

int dialog_handler( int ops, int ok_or_yes, int dialog_code,
                    int with_entry, char *entry_default,
                    char *entered, int answer_length )
{
	static GtkWidget * window, *vbox, *hbox, *label, *entry, *button, *separator;
	static int answer, saved_with_entry;
	static char *saved_entered;
	int id;

	switch ( ops ) {
	case WIDGET_CREATE :
		saved_entered = entered;
		saved_with_entry = with_entry;

		window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
		gtk_window_set_position( GTK_WINDOW( window ), GTK_WIN_POS_CENTER );
		gtk_window_set_title( GTK_WINDOW( window ), dialog_data[ dialog_code ].title );
		id = gtk_signal_connect( GTK_OBJECT( window ), "destroy",
		                         GTK_SIGNAL_FUNC( gtk_main_quit ), NULL );
		vbox = gtk_vbox_new( FALSE, 3 );
		gtk_container_border_width( GTK_CONTAINER( vbox ), 0 );
		gtk_container_add( GTK_CONTAINER( window ), vbox );

		hbox = gtk_hbox_new( FALSE, 0 );
		gtk_container_border_width( GTK_CONTAINER( hbox ), 2 );
		gtk_box_pack_start( GTK_BOX( vbox ), hbox, TRUE, TRUE, 0 );

		label = gtk_label_new( dialog_data[ dialog_code ].msg );
		gtk_box_pack_start( GTK_BOX( hbox ), label, TRUE, TRUE, 0 );

		if ( with_entry ) {
			entry = gtk_entry_new_with_max_length( answer_length );
			gtk_entry_set_text( GTK_ENTRY( entry ), entry_default );
			gtk_editable_select_region( GTK_EDITABLE( entry ), 0, -1 );
			gtk_box_pack_start( GTK_BOX( vbox ), entry, FALSE, FALSE, 0 );
		}
		else if ( entry_default != NULL ) {
			label = gtk_label_new( entry_default );
			gtk_box_pack_start( GTK_BOX( vbox ), label, TRUE, TRUE, 0 );
		}

		separator = gtk_hseparator_new();
		gtk_box_pack_start( GTK_BOX( vbox ), separator, FALSE, FALSE, 0 );

		hbox = gtk_hbox_new( TRUE, 0 );
		gtk_container_border_width( GTK_CONTAINER( hbox ), 3 );
		gtk_box_pack_start( GTK_BOX( vbox ), hbox, TRUE, TRUE, 0 );

		if ( ok_or_yes == TRUE )
			button = gtk_button_new_with_label( "OK" );
		else
			button = gtk_button_new_with_label( "Yes" );
		GTK_WIDGET_SET_FLAGS( button, GTK_CAN_DEFAULT );
		gtk_signal_connect( GTK_OBJECT( button ), "clicked",
		                    GTK_SIGNAL_FUNC( dl_callback ), NULL );
		gtk_box_pack_start( GTK_BOX( hbox ), button, TRUE, TRUE, 0 );
		gtk_widget_grab_default( button );

		if ( ok_or_yes == TRUE )
			button = gtk_button_new_with_label( "cancel" );
		else
			button = gtk_button_new_with_label( "No" );
		GTK_WIDGET_SET_FLAGS( button, GTK_CAN_DEFAULT );
		gtk_signal_connect( GTK_OBJECT( button ), "clicked",
		                    GTK_SIGNAL_FUNC( gtk_main_quit ), NULL );
		gtk_box_pack_start( GTK_BOX( hbox ), button, TRUE, TRUE, 0 );

		gtk_widget_show_all( window );
		answer = FALSE;
		gtk_main();
		gtk_signal_disconnect( GTK_OBJECT( window ), id );
		gtk_widget_destroy( window );
		return answer;

	case DL_OK_PRESSED :
		if ( saved_with_entry == FALSE ) {
			answer = TRUE;
			gtk_main_quit();
			return TRUE;
		}
		if ( strlen( gtk_entry_get_text( GTK_ENTRY( entry ) ) ) == 0 )
			return FALSE;
		strcpy( saved_entered, gtk_entry_get_text( GTK_ENTRY( entry ) ) );
		answer = TRUE;
		gtk_main_quit();
		return TRUE;
	}
	/* Just to avoid compile time warning */
	return FALSE;
}

void status_handler( int status_code, const char *extra_msg )
{
	GtkWidget * window, *vbox, *label, *separator, *button;

	window = gtk_window_new( GTK_WINDOW_DIALOG );
	gtk_window_set_position( GTK_WINDOW( window ), GTK_WIN_POS_CENTER );
	gtk_window_set_title( GTK_WINDOW( window ), "Status" );
	gtk_signal_connect( GTK_OBJECT( window ), "destroy",
	                    GTK_SIGNAL_FUNC( gtk_widget_destroyed ),
	                    &window );
	gtk_container_border_width( GTK_CONTAINER( window ), 5 );

	vbox = gtk_vbox_new( FALSE, 3 );
	gtk_container_border_width( GTK_CONTAINER( vbox ), 0 );
	gtk_container_add( GTK_CONTAINER( window ), vbox );

	label = gtk_label_new( status_msg[ status_code ] );
	gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
	gtk_box_pack_start( GTK_BOX( vbox ), label, TRUE, FALSE, 0 );

	if ( extra_msg != NULL ) {
		label = gtk_label_new( extra_msg );
		gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_LEFT );
		gtk_box_pack_start( GTK_BOX( vbox ), label, TRUE, FALSE, 0 );
	}

	separator = gtk_hseparator_new();
	gtk_box_pack_start( GTK_BOX( vbox ), separator, FALSE, FALSE, 0 );

	button = gtk_button_new_with_label( "Ok" );
	GTK_WIDGET_SET_FLAGS( button, GTK_CAN_DEFAULT );
	gtk_signal_connect_object( GTK_OBJECT( button ), "clicked",
	                           GTK_SIGNAL_FUNC( gtk_widget_destroy ),
	                           GTK_OBJECT( window ) );
	gtk_box_pack_start( GTK_BOX( vbox ), button, TRUE, FALSE, 0 );
	gtk_widget_grab_default( button );

	gtk_widget_show_all( window );
}
