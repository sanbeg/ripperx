bin_PROGRAMS = ripperX

ripperX_SOURCES = \
	cddb.c cddb.h \
	cddbp.c cddbp.h \
	common.h \
	config_rw.c config_rw.h \
	config_window_handler.c config_window_handler.h \
	dir_window_handler.c dir_window_handler.h \
	err_dialog_handler.c err_dialog_handler.h \
	interface_common.h \
	job_control.c job_control.h \
        main.c main.h \
        main_window_handler.c main_window_handler.h \
        misc_utils.c misc_utils.h \
        players_manipulation.c players_manipulation.h \
        ripper_encoder_manipulation.c ripper_encoder_manipulation.h \
        select_frame_handler.c select_frame_handler.h \
        status_frame_handler.c status_frame_handler.h \
        xpms.c

EXTRA_DIST = \
	xpms/cddb.xpm \
	xpms/config.xpm \
	xpms/doall.xpm \
	xpms/exit.xpm \
	xpms/go.xpm \
	xpms/ripperX-icon.xpm \
	xpms/ripperX.xpm \
	xpms/ripperX_box.xpm \
	xpms/s_button_checked.xpm \
	xpms/s_button_unchecked.xpm \
	xpms/s_play_button_disabled.xpm \
	xpms/s_play_button_enabled.xpm \
	xpms/scan.xpm \
	xpms/stop.xpm

INCLUDES = \
	-I$(includedir)

ripperX_LDADD = \
	$(INTLLIBS) \
	$(GTK_LIBS)

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"ripperX\" \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DDATAROOTDIR=\""$(datarootdir)"\" \
	$(GTK_CFLAGS)


