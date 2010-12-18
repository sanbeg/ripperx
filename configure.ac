#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([ripperX], [2.7.3], [tmancill@users.sourceforge.net])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
m4_if(PKG_CONFIG, [no],
      AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/]))

# Checks for header files.
AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT
AC_CHECK_HEADERS([fcntl.h locale.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC

m4_foreach([func],
           [dup2, gethostbyname, memset, mkdir, rmdir, setenv, setlocale, socket,
            strchr, strcspn, strdup, strerror, strncasecmp, strndup, strstr],
           AC_CHECK_FUNC(func,, AC_MSG_ERROR([Missing required function: func])) )

AC_CHECK_FUNC(openpty,, 
               AC_CHECK_LIB(util,openpty,, AC_MSG_ERROR([Missing required function: openpty])) )


# Additional libraries
AM_GNU_GETTEXT([external])

AC_CHECK_LIB(id3, ID3Tag_Link,, AC_MSG_ERROR([Missing required library: id3]))

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.6 gmodule-2.0 >= 2.6 gobject-2.0 >= 2.6 gthread-2.0 >= 2.6)
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.6)

# --disable-deprecated switch for GTK2 purification
AC_ARG_ENABLE(deprecated, [  --disable-deprecated  Disable deprecated GTK functions. ],
			  [GTK_CFLAGS="$GTK_CFLAGS -DG_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED"], [])


AC_CONFIG_FILES([Makefile
                 ripperX.pc
                 po/Makefile.in
                 plugins/Makefile
                 src/Makefile])
AC_OUTPUT

