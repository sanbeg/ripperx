#ifndef MAIN_H
#define MAIN_H

#include "config.h"

#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif

#include "common.h"

#include <locale.h>

#include <gtk/gtk.h>


void ripperX_exit(GtkWidget *widget, gpointer callback_data);

#endif
