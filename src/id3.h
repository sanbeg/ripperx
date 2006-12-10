/* id3.h
 *
 * ID3 format
 *
 * Part of id3tool
 *
 * Copyright (C) 1999, Christopher Collins
*/

/*  id3tool:  An editor for ID3 tags.
 *  Copyright (C) 1999  Christopher Collins
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _ID3_H
#define _ID3_H

#include "common.h"
#include <stdio.h>

typedef struct id3tag_s {
	char	magic[ 3 ];
	char	songname[ 30 ];
	char	artist[ 30 ];
	char	album[ 30 ];
	char	year[ 4 ];
	char	note[ 28 ];
	unsigned char	null;
	unsigned char	track;
	unsigned char	style;
} id3tag_t;

struct style_s {
	unsigned char	styleid;
	char	*name;
};

/*
	  moved the typedef of cddb_category_s and cddb_categories to 
	  header file to gain access in cddb.c -- Carl Seutter
*/
typedef struct cddb_category_s {
	unsigned char styleid;
	char *cddb_name;
} cddb_category_t;

extern cddb_category_t cddb_categories[];

extern struct style_s	id3_styles[];

extern int	id3_readtag ( FILE *fin, id3tag_t *id3tag );
extern int	id3_appendtag ( FILE *fout, id3tag_t *id3tag );
extern int	id3_replacetag ( FILE *fout, id3tag_t *id3tag );
extern void	id3_cleartag( id3tag_t *id3tag );
extern char	*id3_findstyle( int styleid );
unsigned char id3_find_cddb_category( char *name );
void vorbistag(char *ogg_file, char *artist, char *album, char *title, unsigned char style, unsigned char track); 

#endif     /* #ifndef _ID3_H */
