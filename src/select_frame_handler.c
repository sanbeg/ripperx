#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>
#include <glib/gi18n.h>
#include <stdio.h>
#include <string.h>

#include "id3.h"

#include "misc_utils.h"
#include "interface_common.h"
#include "main_window_handler.h"
#include "players_manipulation.h"

#include "select_frame_handler.h"

#include "xpms/s_play_button_enabled.xpm"
#include "xpms/s_play_button_disabled.xpm"
#include "xpms/s_button_checked.xpm"
#include "xpms/s_button_unchecked.xpm"


void sf_select_all_button_clicked( GtkWidget *widget, gpointer callback_data );
void sf_select_button_toggled( GtkWidget *widget, gpointer callback_data );
void sf_filename_entry_changed( GtkWidget *widget, gpointer callback_data );
void sf_cd_play_button_clicked( GtkWidget *widget, gpointer callback_data );


void sf_select_all_button_clicked( GtkWidget *widget, gpointer callback_data )
{
	select_frame_handler( SF_SELECT_BUTTON_ACT_ALL, 0, NULL );
}

void sf_track_selected_button_toggled( GtkWidget *widget, gpointer callback_data )
{
	int track;

	track = *( int * ) callback_data;
	select_frame_handler( SF_SELECT_BUTTON_ACT, track, NULL );
}

void sf_artist_entry_changed( GtkWidget *widget, gpointer callback_data )
{
	select_frame_handler( SF_ARTIST_ENTRY_CHG, 0, NULL );
}

void sf_album_entry_changed( GtkWidget *widget, gpointer callback_data )
{
	select_frame_handler( SF_ALBUM_ENTRY_CHG, 0, NULL );
}

// patch from M.Tyler
// tm:  we're not quite read for this (need updates to cddb.c)
void sf_year_entry_changed( GtkWidget *widget, gpointer callback_data )
{
	select_frame_handler( SF_YEAR_ENTRY_CHG, 0, NULL );
}

void sf_genre_entry_changed( GtkWidget *widget, gpointer callback_data )
{
	select_frame_handler( SF_GENRE_ENTRY_CHG, 0, NULL );
}

void sf_filename_entry_changed( GtkWidget *widget, gpointer callback_data )
{
	int track;

	track = *( int * ) callback_data;
	select_frame_handler( SF_FILENAME_ENTRY_CHG, track, NULL );
}

void sf_cd_play_button_clicked( GtkWidget *widget, gpointer callback_data )
{
	char buf[ 5 ];

	play_cd_wav_mp3( STOP, CD, NULL );
	play_cd_wav_mp3( STOP, WAV, NULL );
	play_cd_wav_mp3( STOP, MP3, NULL );

	snprintf( buf, sizeof( buf ), "%d", ( *( int * ) callback_data ) + 1 );
	play_cd_wav_mp3( PLAY, CD, buf );
}

void select_frame_handler( int ops, int track, _main_data *main_data )
{
	static GtkWidget * select_frame = NULL;
	static GdkPixmap *play_button_enabled_pixmap = NULL;
	static GdkPixmap *play_button_disabled_pixmap = NULL;
	static GdkPixmap *button_checked_pixmap = NULL;
	static GdkPixmap *button_unchecked_pixmap = NULL;
	static GdkBitmap *play_button_enabled_mask;
	static GdkBitmap *play_button_disabled_mask;
	static GdkBitmap *button_checked_mask = NULL;
	static GdkBitmap *button_unchecked_mask = NULL;
	static GtkWidget *select_all_button = NULL;
	static GtkWidget *track_selected_button[ MAX_NUM_TRACK ];
	static GtkWidget *track_selected_button_pixmap[ MAX_NUM_TRACK ];
	static GtkWidget *artist_label = NULL;
	static GtkWidget *artist_entry = NULL;
	static GtkWidget *album_label = NULL;
	static GtkWidget *album_entry = NULL;

	static GtkWidget *year_label = NULL;
	static GtkWidget *year_entry = NULL;
	static GtkWidget *genre_label = NULL;
	static GtkWidget *genre_entry = NULL;
	//static GtkWidget *genre_combo = NULL;
	//static GList *genre_combo_items = NULL;

	static GtkWidget *filename_entry[ MAX_NUM_TRACK ];
	static GtkWidget *rip_action_button = NULL;
	static GtkWidget *encode_action_button = NULL;
	static char saved_filename_entry[ MAX_NUM_TRACK ][ MAX_FILE_NAME_LENGTH ];
	static int track_numbers[ MAX_NUM_TRACK ];
	static _main_data *saved_main_data;
	static int num_tracks, button_state;
	GtkWidget *pixmap;

	switch ( ops ) {
	case WIDGET_CREATE : {
			GtkWidget * main_frame, *vbox, *hbox1, *vbox2, *hbox, *hbox2, *hbox3, *hbox4, *hbox5, *hbox6, *label, *scr_window;
			GtkWidget *table, *separator, *button;
			GtkStyle *style;
			GdkGC *gc;
			char buf[ 5 ];
			char *readable_length;
			int i;

			if ( select_frame != NULL )
				return;

			main_frame = main_window_handler( MW_REQUEST_MF, 0, NULL );
			num_tracks = main_data->num_tracks;
			saved_main_data = main_data;

			if ( play_button_enabled_pixmap == NULL ) {
				/* Create pixmaps */
				style = gtk_widget_get_default_style();
				gc = style->black_gc;
				play_button_enabled_pixmap
				= gdk_pixmap_create_from_xpm_d( main_frame->window,
				                                &play_button_enabled_mask,
				                                &style->bg[ GTK_STATE_NORMAL ],
				                                s_play_button_enabled_xpm );
				play_button_disabled_pixmap
				= gdk_pixmap_create_from_xpm_d( main_frame->window,
				                                &play_button_disabled_mask,
				                                &style->bg[ GTK_STATE_NORMAL ],
				                                s_play_button_disabled_xpm );
				button_checked_pixmap
				= gdk_pixmap_create_from_xpm_d( main_frame->window,
				                                &button_checked_mask,
				                                &style->bg[ GTK_STATE_NORMAL ],
				                                s_button_checked_xpm );
				button_unchecked_pixmap
				= gdk_pixmap_create_from_xpm_d( main_frame->window,
				                                &button_unchecked_mask,
				                                &style->bg[ GTK_STATE_NORMAL ],
				                                s_button_unchecked_xpm );

			}

			/* Create a vbox. Don't show it yet */
			vbox = gtk_vbox_new( FALSE, 0 );
			select_frame = vbox;
			gtk_container_add( GTK_CONTAINER( main_frame ), vbox );

			/* Create a hbox & labels and pack */
			hbox = gtk_hbox_new( FALSE, 0 );
			gtk_box_pack_start( GTK_BOX( vbox ), hbox, FALSE, FALSE, 0 );

			label = gtk_label_new( " " );
			gtk_widget_set_usize( label, 80, 0 );
			gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 0 );

			select_all_button = gtk_toggle_button_new();
			hbox2 = gtk_hbox_new( FALSE, 0 );
			gtk_container_set_border_width( GTK_CONTAINER( hbox2 ), 0 );
			gtk_container_add( GTK_CONTAINER( select_all_button ), hbox2 );

			pixmap = gtk_pixmap_new( button_checked_pixmap,
			                         button_checked_mask );
			gtk_widget_set_usize( pixmap, 15, 0 );
			gtk_box_pack_start( GTK_BOX( hbox2 ), pixmap, FALSE, FALSE, 0 );

			label = gtk_label_new(_("Select All Tracks"));
			gtk_widget_set_usize( label, 120, 0 );
			gtk_box_pack_end( GTK_BOX( hbox2 ), label, FALSE, FALSE, 0 );

			g_signal_connect( G_OBJECT( select_all_button ), "clicked",
			                    G_CALLBACK( sf_select_all_button_clicked ),
			                    NULL );
			gtk_box_pack_start( GTK_BOX( hbox ), select_all_button, FALSE, FALSE, 0 );

			/* action buttons */
			hbox1 = gtk_hbox_new ( FALSE, 0 );
			gtk_container_set_border_width( GTK_CONTAINER( hbox1 ), 5 );
			gtk_box_pack_start( GTK_BOX( hbox ), hbox1 , FALSE, FALSE, 0 );

			rip_action_button = gtk_radio_button_new_with_label( NULL, _("Rip to WAV") );
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( rip_action_button ), 0 );
			gtk_box_pack_start( GTK_BOX( hbox1 ), rip_action_button, FALSE, FALSE, 0 );

			encode_action_button = gtk_radio_button_new_with_label(
			                           gtk_radio_button_group( GTK_RADIO_BUTTON( rip_action_button ) ), _("Encode") );
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( encode_action_button ), 1 );
			gtk_box_pack_start( GTK_BOX( hbox1 ), encode_action_button, FALSE, FALSE, 0 );

			/* Create a scrolled window */
			scr_window = gtk_scrolled_window_new( NULL, NULL );
			gtk_container_set_border_width( GTK_CONTAINER( scr_window ), 5 );
			gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scr_window ),
			                                GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS );
			gtk_box_pack_start( GTK_BOX( vbox ), scr_window, TRUE, TRUE, 0 );

			/* Create table and vbox which will contain the table */
			vbox2 = gtk_vbox_new( FALSE, 0 );
			gtk_container_set_border_width( GTK_CONTAINER( vbox2 ), 0 );
			gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW( scr_window ), vbox2 );

			/* artist entry fields */
			hbox3 = gtk_hbox_new (FALSE, 0);

			artist_label = gtk_label_new(_("Artist "));
			gtk_box_pack_start(GTK_BOX(hbox3), artist_label, FALSE, FALSE, 5);
			artist_entry = gtk_entry_new_with_max_length(MAX_ARTIST_LENGTH);
			gtk_entry_set_text(GTK_ENTRY(artist_entry), main_data->disc_artist);
			g_signal_connect( G_OBJECT( artist_entry ), "changed",
			                    G_CALLBACK( sf_artist_entry_changed ),
			                    NULL );
			gtk_box_pack_start(GTK_BOX(hbox3), artist_entry, TRUE, TRUE, 5);
			gtk_box_pack_start(GTK_BOX(vbox2), hbox3, FALSE, FALSE, 5);

			/* album entry fields */
			hbox4 = gtk_hbox_new (FALSE, 0);
			album_label = gtk_label_new(_("Album"));
			gtk_box_pack_start(GTK_BOX(hbox4), album_label, FALSE, FALSE, 5);
			album_entry = gtk_entry_new_with_max_length(MAX_ARTIST_LENGTH);
			gtk_entry_set_text(GTK_ENTRY(album_entry), main_data->disc_title);
			g_signal_connect( G_OBJECT( album_entry ), "changed",
			                    G_CALLBACK( sf_album_entry_changed ),
			                    NULL );
			gtk_box_pack_start(GTK_BOX(hbox4), album_entry, TRUE, TRUE, 5);
			gtk_box_pack_start(GTK_BOX(vbox2), hbox4, FALSE, FALSE, 5);

			/* year and genre entry fields */
			hbox5 = gtk_hbox_new (FALSE, 0);

			year_label = gtk_label_new(_("Year"));
			gtk_box_pack_start(GTK_BOX(hbox5), year_label, FALSE, FALSE, 5);
			year_entry = gtk_entry_new_with_max_length(MAX_YEAR_LENGTH);
			gtk_widget_set_usize(year_entry, 60, -2);
			if ( strlen(main_data->disc_year) < 2 )
				sprintf(main_data->disc_year, "2007");
			gtk_entry_set_text(GTK_ENTRY(year_entry), main_data->disc_year);
			g_signal_connect( G_OBJECT( year_entry ), "changed",
			                  G_CALLBACK( sf_year_entry_changed ),
			                  NULL );
			gtk_box_pack_start(GTK_BOX(hbox5), year_entry, TRUE, TRUE, 5);

			genre_label = gtk_label_new(_("Genre"));
			gtk_box_pack_start(GTK_BOX(hbox5), genre_label, FALSE, FALSE, 5);
			genre_entry = gtk_entry_new_with_max_length(MAX_GENRE_LENGTH);
			gtk_entry_set_text(GTK_ENTRY(genre_entry), main_data->disc_category);
			g_signal_connect( G_OBJECT( genre_entry ), "changed",
					  G_CALLBACK( sf_genre_entry_changed ),
					  NULL );
			gtk_box_pack_start(GTK_BOX(hbox5), genre_entry, TRUE, TRUE, 5);
			gtk_box_pack_start(GTK_BOX(vbox2), hbox5, FALSE, FALSE, 5);

//			genre_combo = gtk_combo_new ();
//			gtk_widget_show (genre_combo);
//			gtk_box_pack_start(GTK_BOX(hbox3), genre_combo, TRUE, TRUE, 5);
//			gtk_widget_set_usize (genre_combo, 80, -2);
//			gtk_container_set_border_width (GTK_CONTAINER (genre_combo), 5);
//			gtk_combo_set_value_in_list (GTK_COMBO (genre_combo), TRUE, FALSE);
//			gtk_combo_set_use_arrows_always (GTK_COMBO (genre_combo), TRUE);

//			i = 0;
//			while ( id3_styles[i].name != NULL )
//			{
//				genre_combo_items = g_list_append (genre_combo_items,
//							(gpointer) id3_styles[i].name);
//				i++;
//			}
//			gtk_combo_set_popdown_strings (GTK_COMBO (genre_combo), genre_combo_items);
//			g_list_free (genre_combo_items);
//			genre_combo_items = NULL;
//
//			genre_entry = GTK_COMBO (genre_combo)->entry;
//
//			if ( strlen(main_data->disc_category) < 2 )
//				sprintf(main_data->disc_category, "Pop");
//
//			gtk_entry_set_text (GTK_ENTRY (genre_entry), main_data->disc_category);
//			gtk_signal_connect( GTK_OBJECT( genre_entry ), "changed",
//			                    GTK_SIGNAL_FUNC( sf_genre_entry_changed ),
//			                    NULL );
//			gtk_widget_show (genre_entry);


			
			/* the table */
			table = gtk_table_new( MAX_NUM_TRACK, 6, FALSE );
			gtk_box_pack_start( GTK_BOX( vbox2 ), table, FALSE, FALSE, 0 );
			gtk_table_set_row_spacings( GTK_TABLE( table ), 1 );
			gtk_table_set_col_spacings( GTK_TABLE( table ), 0 );
			gtk_container_set_border_width( GTK_CONTAINER( table ), 5 );

			/* Separators */
			if ( num_tracks > 0 ) {
				separator = gtk_vseparator_new();
				gtk_widget_set_usize( separator, 10, 0 );
				gtk_table_attach( GTK_TABLE( table ), separator, 3, 4, 0, num_tracks,
				                  0, GTK_FILL, 0, 0 );
			}

			for ( i = 0; i < num_tracks; i++ ) {
				track_numbers[ i ] = i;
				if ( strlen ( saved_filename_entry[ i ] ) == 0 ) {
					get_track_title(saved_filename_entry[i], main_data, i);
				}

				/* CD play button */
				button = gtk_button_new();
				pixmap = gtk_pixmap_new( play_button_enabled_pixmap,
				                         play_button_enabled_mask );
				gtk_widget_set_usize( button, 18, 18 );
				gtk_container_add( GTK_CONTAINER( button ), pixmap );

				g_signal_connect( G_OBJECT( button ), "clicked",
				                    G_CALLBACK( sf_cd_play_button_clicked ),
				                    &track_numbers[ i ] );
				gtk_table_attach( GTK_TABLE( table ), button, 0, 1, i, i + 1,
				                  0, GTK_EXPAND | GTK_FILL, 0, 0 );

				/* Track number */
				sprintf( buf, "%d", i + 1 );
				label = gtk_label_new( buf );
				gtk_widget_set_usize( label, 20, 0 );
				gtk_table_attach( GTK_TABLE( table ), label, 1, 2, i, i + 1,
				                  0, 0, 0, 0 );
				/* Length */
				readable_length = length_to_readable( main_data->track[ i ].length );
				label = gtk_label_new( readable_length );
				gtk_table_attach( GTK_TABLE( table ), label, 2, 3, i, i + 1,
				                  0, 0, 0, 0 );

				/* MP3 check buttons */
				track_selected_button[ i ] = gtk_toggle_button_new();
				gtk_widget_set_usize( track_selected_button[ i ], 18, 18 );
				track_selected_button_pixmap[ i ] = gtk_pixmap_new( button_unchecked_pixmap,
				                                    button_unchecked_mask );
				gtk_container_add( GTK_CONTAINER( track_selected_button[ i ] ), track_selected_button_pixmap[ i ] );
				gtk_table_attach( GTK_TABLE( table ), track_selected_button[ i ], 4, 5, i, i + 1,
				                  0, GTK_EXPAND | GTK_FILL, 0, 0 );

				/* file name entry */
				filename_entry[ i ] = gtk_entry_new_with_max_length( MAX_FILE_NAME_LENGTH );
				gtk_widget_set_usize( filename_entry[ i ], 325, 0 );
				gtk_entry_set_text( GTK_ENTRY( filename_entry[ i ] ), saved_filename_entry[ i ] );
				gtk_table_attach( GTK_TABLE( table ), filename_entry[ i ], 5, 6, i, i + 1,
				                  GTK_EXPAND | GTK_FILL, 0, 0, 0 );

				g_signal_connect( G_OBJECT( track_selected_button[ i ] ), "clicked",
				                    G_CALLBACK( sf_track_selected_button_toggled ),
				                    &track_numbers[ i ] );
				g_signal_connect( G_OBJECT( filename_entry[ i ] ), "changed",
				                    G_CALLBACK( sf_filename_entry_changed ),
				                    &track_numbers[ i ] );
			}
			gtk_widget_show_all( vbox );
			return;
		}

	case WIDGET_DESTROY :
		if ( select_frame == NULL )
			return;
		gtk_widget_destroy( select_frame );
		select_frame = NULL;
		return;

	case CLEAR_ENTRIES :
		/* clear filename entries */
		for ( track = 0; track < num_tracks; track++ )
			saved_filename_entry[ track ][ 0 ] = 0;
		return;

	case SF_SELECT_BUTTON_ACT :
		if ( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ) ->active ) {
			if ( strlen( saved_filename_entry[ track ] ) == 0 ) {
				get_track_title(saved_filename_entry[track], saved_main_data, track);
			}
			gtk_entry_set_text( GTK_ENTRY( filename_entry[ track ] ),
			                    saved_filename_entry[ track ] );

			gtk_widget_destroy( track_selected_button_pixmap[ track ] );
			track_selected_button_pixmap[ track ] = gtk_pixmap_new( button_checked_pixmap,
			                                        button_checked_mask );
			gtk_container_add( GTK_CONTAINER( track_selected_button[ track ] ),
			                   track_selected_button_pixmap[ track ] );
			gtk_widget_show( track_selected_button_pixmap[ track ] );
		} else {
			gtk_widget_destroy( track_selected_button_pixmap[ track ] );
			track_selected_button_pixmap[ track ] = gtk_pixmap_new( button_unchecked_pixmap,
			                                        button_unchecked_mask );
			gtk_container_add( GTK_CONTAINER( track_selected_button[ track ] ),
			                   track_selected_button_pixmap[ track ] );
			gtk_widget_show( track_selected_button_pixmap[ track ] );
		}
		return;

	case SF_ARTIST_ENTRY_CHG :
		/* The artist name has been edited. Sync saved artist name */
		strcpy( saved_main_data->disc_artist,
		        gtk_entry_get_text( GTK_ENTRY( artist_entry ) ) );
		return;

	case SF_ALBUM_ENTRY_CHG :
		/* The album name has been edited. Sync saved album name */
		strcpy( saved_main_data->disc_title,
		        gtk_entry_get_text( GTK_ENTRY( album_entry ) ) );
		return;

	case SF_YEAR_ENTRY_CHG :
		/* The album year has been edited. Sync saved album year */
		strcpy( saved_main_data->disc_year,
		        gtk_entry_get_text( GTK_ENTRY( year_entry ) ) );
		return;

	case SF_GENRE_ENTRY_CHG :
		/* The album genre has been edited. Sync saved album genre */
		strcpy( saved_main_data->disc_category,
		        gtk_entry_get_text( GTK_ENTRY( genre_entry ) ) );
		return;

	case SF_FILENAME_ENTRY_CHG :
		if ( strlen( gtk_entry_get_text( GTK_ENTRY( filename_entry[ track ] ) ) ) == 0 ) {
			if ( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ) ->active == TRUE ) {
				/* The user has deleted the file name. Untoggle the button and
				 * delete saved_filename_entry too */
				saved_filename_entry[ track ][ 0 ] = '\0';
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ),
				                             FALSE );
			}
		}
		else {
			/* The file name has been edited. Sync saved file name */
			strcpy( saved_filename_entry[ track ],
			        gtk_entry_get_text( GTK_ENTRY( filename_entry[ track ] ) ) );
			/* 
			 * 20051030/tm
			 * no longer auto-select the track when the text is altered
			 * 
			 
			if ( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ) ->active == FALSE )
				gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ),
				                             TRUE );
			*/
		}
		return;

	case SF_SELECT_BUTTON_ACT_ALL :
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( select_all_button ), GTK_TOGGLE_BUTTON( select_all_button ) ->active );
		for ( track = 0; track < num_tracks; track++ ) {
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ),
			                             GTK_TOGGLE_BUTTON( select_all_button ) ->active );
		}
		return;

	case SF_SYNC_MAIN_DATA :
		main_data = saved_main_data;

		for ( track = 0; track < num_tracks; track++ ) {
            char *tmp = saved_filename_entry[ track ];

			if (strlen(tmp)) {

				/* hack for l3enc. It is so stupid about spaces in the file name */
				if ( !strcmp( config.encoder.encoder, "l3enc" ) )
					convert_spaces( tmp, '_' );

				/* put track title back into main_data */
				put_track_title(tmp, main_data, track);
			}

			if ( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ) ->active == TRUE ) {
				if ( GTK_TOGGLE_BUTTON( rip_action_button ) ->active ) {
					main_data->track[ track ].make_wav = TRUE;
					main_data->track[ track ].make_mp3 = FALSE;
				} else {
					main_data->track[ track ].make_wav = FALSE;
					main_data->track[ track ].make_mp3 = TRUE;
				}
			}
		}
		return;

	case SF_SYNC_SELECT_FRAME :

		main_data = saved_main_data;

		gtk_entry_set_text(GTK_ENTRY(artist_entry), main_data->disc_artist);
		gtk_entry_set_text(GTK_ENTRY(album_entry), main_data->disc_title);
		gtk_entry_set_text(GTK_ENTRY(year_entry), main_data->disc_year);
		gtk_entry_set_text(GTK_ENTRY(genre_entry), main_data->disc_category);

		for ( track = 0; track < num_tracks; track++ ) {
			button_state = GTK_TOGGLE_BUTTON( track_selected_button[ track ] ) ->active;
			get_track_title(saved_filename_entry[track], main_data, track);
			gtk_entry_set_text( GTK_ENTRY( filename_entry[ track ] ),
			                    saved_filename_entry[ track ] );
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( track_selected_button[ track ] ),
			                             button_state );
		}
		return;
	}
}
