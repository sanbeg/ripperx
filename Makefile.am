AUTOMAKE_OPTIONS = dist-bzip2

SUBDIRS = po src plugins

man_MANS = \
     ripperX.1

EXTRA_DIST = config/config.rpath m4/ChangeLog  \
	BUGS \
	CHANGES \
	FAQ \
	README.plugin \
	README.plugin_spec_v0.1 \
	README.plugin_tester \
	bootstrap \
	ripperX.desktop \
	ripperX.pc.in \
	$(man_MANS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ripperX.pc


ACLOCAL_AMFLAGS = -I m4
