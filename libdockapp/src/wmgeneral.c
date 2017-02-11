/*
	wmgeneral was taken from wmppp.

	It has a lot of routines which most of the wm* programs use.

	------------------------------------------------------------

	Copyright (C) 1998 Martijn Pieterse (pieterse@xs4all.nl)

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
	02110-1301, USA.

	---
	CHANGES:
	---
	10/10/2003 (Simon Law, sfllaw@debian.org)
		* changed the parse_rcfile function to use getline instead of
		  fgets.
	10/14/2000 (Chris Gray, cgray@tribsoft.com)
		* Removed a bug from parse_rcfile.  An extra
		  newline would cause a segfault.
	14/09/1998 (Dave Clark, clarkd@skyia.com)
		* Updated createXBMfromXPM routine
		* Now supports >256 colors
	11/09/1998 (Martijn Pieterse, pieterse@xs4all.nl)
		* Removed a bug from parse_rcfile. You could
		  not use "start" in a command if a label was
		  also start.
		* Changed the needed geometry string.
		  We don't use window size, and don't support
		  negative positions.
	03/09/1998 (Martijn Pieterse, pieterse@xs4all.nl)
		* Added parse_rcfile2
	02/09/1998 (Martijn Pieterse, pieterse@xs4all.nl)
		* Added -geometry support (untested)
	28/08/1998 (Martijn Pieterse, pieterse@xs4all.nl)
		* Added createXBMfromXPM routine
		* Saves a lot of work with changing xpm's.
	02/05/1998 (Martijn Pieterse, pieterse@xs4all.nl)
		* changed the read_rc_file to parse_rcfile, as suggested by
		  Marcelo E. Magallon
		* debugged the parse_rc file.
	30/04/1998 (Martijn Pieterse, pieterse@xs4all.nl)
		* Ripped similar code from all the wm* programs,
		  and put them in a single file.

*/

#define _POSIX_C_SOURCE 200809L
#include "wmgeneral.h"
#include <X11/Xlib.h>                   /* for XCopyArea, etc */
#include <X11/Xutil.h>                  /* for XSizeHints, XWMHints, etc */
#include <X11/extensions/shape.h>       /* for XShapeCombineMask */
#include <X11/extensions/shapeconst.h>  /* for ShapeBounding, ShapeSet */
#include <X11/xpm.h>                    /* for XpmAttributes, Pixel, etc */
#include <stddef.h>                     /* for size_t */
#include <stdio.h>                      /* for fprintf, stderr, NULL, etc */
#include <stdlib.h>                     /* for exit, free */
#include <string.h>                     /* for strcmp, strdup, strcspn, etc */

  /*****************/
 /* X11 Variables */
/*****************/

Window		Root;
int			screen;
XSizeHints	mysizehints;
XWMHints	mywmhints;
Pixel		back_pix, fore_pix;
GC			NormalGC;
XpmIcon		wmgen;
Pixmap		pixmask;

  /*****************/
 /* Mouse Regions */
/*****************/

typedef struct {
	int		enable;
	int		top;
	int		bottom;
	int		left;
	int		right;
} MOUSE_REGION;

MOUSE_REGION	mouse_region[MAX_MOUSE_REGION];

  /***********************/
 /* Function Prototypes */
/***********************/

static void GetXPM(XpmIcon *, char **);
static Pixel GetColor(char *);
void RedrawWindow(void);
void AddMouseRegion(int, int, int, int, int);
int CheckMouseRegion(int, int);

/*******************************************************************************\
|* parse_rcfile																   *|
\*******************************************************************************/

void parse_rcfile(const char *filename, rckeys *keys) {

	char	*p;
	FILE	*fp;

	fp = fopen(filename, "r");
	if (fp) {
		char temp[128];

		while (fgets(temp, 128, fp)) {
			char *q, *saveptr;
			char *tokens = " :\t\n";
			int key;

			key = 0;
			q = strdup(temp);
			q = strtok_r(q, tokens, &saveptr);
			if(!q)
				continue;
			while (key >= 0 && keys[key].label) {
				if ((!strcmp(q, keys[key].label))) {
					int i;

					p = strstr(temp, keys[key].label);
					p += strlen(keys[key].label);
					p += strspn(p, tokens);
					if ((i = strcspn(p, "#\n"))) p[i] = '\0';
					*keys[key].var = strdup(p);
					key = -1;
				} else key++;
			}
		}
		fclose(fp);
	}
}

/*******************************************************************************\
|* parse_rcfile2															   *|
\*******************************************************************************/

void parse_rcfile2(const char *filename, rckeys2 *keys) {

	char	*p;
	char	*line = NULL;
	size_t  line_size = 0;
	FILE	*fp;

	fp = fopen(filename, "r");
	if (fp) {
		while (getline(&line, &line_size, fp) >= 0) {
			int key;

			key = 0;
			while (key >= 0 && keys[key].label) {
				if ((p = strstr(line, keys[key].label))) {
					char *tokens = " :\t\n";
					int i;

					p += strlen(keys[key].label);
					p += strspn(p, tokens);
					if ((i = strcspn(p, "#\n"))) p[i] = 0;
					*keys[key].var = strdup(p);
					key = -1;
				} else key++;
			}
		}
		fclose(fp);
	}
}


/*******************************************************************************\
|* GetXPM																	   *|
\*******************************************************************************/

static void GetXPM(XpmIcon *wmgen, char *pixmap_bytes[]) {

	XWindowAttributes	attributes;
	int					err;

	/* For the colormap */
	XGetWindowAttributes(DADisplay, Root, &attributes);

	wmgen->attributes.valuemask |= (XpmReturnPixels | XpmReturnExtensions);

	err = XpmCreatePixmapFromData(DADisplay, Root, pixmap_bytes, &(wmgen->pixmap),
					&(wmgen->mask), &(wmgen->attributes));

	if (err != XpmSuccess) {
		fprintf(stderr, "Not enough free colorcells.\n");
		exit(1);
	}
}

/*******************************************************************************\
|* GetColor																	   *|
\*******************************************************************************/

static Pixel GetColor(char *name) {

	XColor				color;
	XWindowAttributes	attributes;

	XGetWindowAttributes(DADisplay, Root, &attributes);

	color.pixel = 0;
	if (!XParseColor(DADisplay, attributes.colormap, name, &color)) {
		fprintf(stderr, "wm.app: can't parse %s.\n", name);
	} else if (!XAllocColor(DADisplay, attributes.colormap, &color)) {
		fprintf(stderr, "wm.app: can't allocate %s.\n", name);
	}
	return color.pixel;
}

/*******************************************************************************\
|* flush_expose																   *|
\*******************************************************************************/

static int flush_expose(Window w) {

	XEvent 		dummy;
	int			i=0;

	while (XCheckTypedWindowEvent(DADisplay, w, Expose, &dummy))
		i++;

	return i;
}

/*******************************************************************************\
|* RedrawWindow																   *|
\*******************************************************************************/

void RedrawWindow(void) {

	flush_expose(DAIcon);
	XCopyArea(DADisplay, wmgen.pixmap, DAIcon, NormalGC,
				0,0, wmgen.attributes.width, wmgen.attributes.height, 0,0);
	flush_expose(DALeader);
	XCopyArea(DADisplay, wmgen.pixmap, DALeader, NormalGC,
				0,0, wmgen.attributes.width, wmgen.attributes.height, 0,0);
}

/*******************************************************************************\
|* RedrawWindowXY															   *|
\*******************************************************************************/

void RedrawWindowXY(int x, int y) {

	flush_expose(DAIcon);
	XCopyArea(DADisplay, wmgen.pixmap, DAIcon, NormalGC,
				x,y, wmgen.attributes.width, wmgen.attributes.height, 0,0);
	flush_expose(DALeader);
	XCopyArea(DADisplay, wmgen.pixmap, DALeader, NormalGC,
				x,y, wmgen.attributes.width, wmgen.attributes.height, 0,0);
}

/*******************************************************************************\
|* AddMouseRegion															   *|
\*******************************************************************************/

void AddMouseRegion(int index, int left, int top, int right, int bottom) {

	if (index < MAX_MOUSE_REGION) {
		mouse_region[index].enable = 1;
		mouse_region[index].top = top;
		mouse_region[index].left = left;
		mouse_region[index].bottom = bottom;
		mouse_region[index].right = right;
	}
}

/*******************************************************************************\
|* CheckMouseRegion															   *|
\*******************************************************************************/

int CheckMouseRegion(int x, int y) {

	int		i;
	int		found;

	found = 0;

	for (i=0; i<MAX_MOUSE_REGION && !found; i++) {
		if (mouse_region[i].enable &&
			x <= mouse_region[i].right &&
			x >= mouse_region[i].left &&
			y <= mouse_region[i].bottom &&
			y >= mouse_region[i].top)
			found = 1;
	}
	if (!found) return -1;
	return (i-1);
}

/*******************************************************************************\
|* createXBMfromXPM															   *|
\*******************************************************************************/
void createXBMfromXPM(char *xbm, char **xpm, int sx, int sy) {

	int	i,j,k;
	int	width, height, numcol, depth;
	int 	zero=0;
	int     curpixel;

	sscanf(*xpm, "%10d %10d %10d %10d", &width, &height, &numcol, &depth);


	for (k=0; k!=depth; k++)
	{
		zero <<=8;
		zero |= xpm[1][k];
	}

	for (i=numcol+1; i < numcol+sy+1; i++) {
		unsigned char bwrite;
		int bcount;

		bcount = 0;
		bwrite = 0;
		for (j=0; j<sx*depth; j+=depth) {
			bwrite >>= 1;

			curpixel=0;
			for (k=0; k!=depth; k++)
			{
				curpixel <<=8;
				curpixel |= xpm[i][j+k];
			}

			if ( curpixel != zero ) {
				bwrite += 128;
			}
			bcount++;
			if (bcount == 8) {
				*xbm = bwrite;
				xbm++;
				bcount = 0;
				bwrite = 0;
			}
		}
	}
}

/*******************************************************************************\
|* copyXPMArea																   *|
\*******************************************************************************/

void copyXPMArea(int x, int y, int sx, int sy, int dx, int dy) {

	XCopyArea(DADisplay, wmgen.pixmap, wmgen.pixmap, NormalGC, x, y, sx, sy, dx, dy);

}

/*******************************************************************************\
|* copyXBMArea																   *|
\*******************************************************************************/

void copyXBMArea(int x, int y, int sx, int sy, int dx, int dy) {

	XCopyArea(DADisplay, wmgen.mask, wmgen.pixmap, NormalGC, x, y, sx, sy, dx, dy);
}


/*******************************************************************************\
|* setMaskXY																   *|
\*******************************************************************************/

void setMaskXY(int x, int y) {

	 XShapeCombineMask(DADisplay, DALeader, ShapeBounding, x, y, pixmask, ShapeSet);
	 XShapeCombineMask(DADisplay, DAIcon, ShapeBounding, x, y, pixmask, ShapeSet);
}

/*******************************************************************************\
|* openXwindow																   *|
\*******************************************************************************/
void openXwindow(int argc, char *argv[], char *pixmap_bytes[], char *pixmask_bits, int pixmask_width, int pixmask_height) {

	char			*display_name = NULL;
	char			*wname = argv[0];

	XGCValues		gcv;
	unsigned long	gcm;

	/* char			*geometry = NULL; */

	int				i;

	for (i=1; argv[i]; i++) {
		if (!strcmp(argv[i], "-display"))
			display_name = argv[++i];
		/* else if (!strcmp(argv[i], "-geometry"))
		 * 	geometry = argv[++i]; */
	}


	DAStoreArguments(argc, argv);
	DAOpenDisplay(display_name, argc, argv);
	display = DADisplay; /* deprecated name */
	screen  = DefaultScreen(DADisplay);
	Root    = RootWindow(DADisplay, screen);

	/* Convert XPM to XImage */
	GetXPM(&wmgen, pixmap_bytes);

	/* TODO - restore ability to specify position from geometry string */
	/* using XWMGeometry */
	DACreateIcon(wname, 64, 64, argc, argv);

	/* TODO - use DASetCallbacks */
	XSelectInput(DADisplay, DALeader, ButtonPressMask | ExposureMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);
	XSelectInput(DADisplay, DAIcon, ButtonPressMask | ExposureMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);

	/* Create GC for drawing */
	/* TODO - use DAGC */
	back_pix = GetColor("white");
	fore_pix = GetColor("black");

	gcm = GCForeground | GCBackground | GCGraphicsExposures;
	gcv.foreground = fore_pix;
	gcv.background = back_pix;
	gcv.graphics_exposures = 0;
	NormalGC = XCreateGC(DADisplay, Root, gcm, &gcv);

	/* ONLYSHAPE ON */
	/* TODO - use DASetShapeWithOffsetForWindow */
	pixmask = XCreateBitmapFromData(DADisplay, DALeader, pixmask_bits, pixmask_width, pixmask_height);

	XShapeCombineMask(DADisplay, DALeader, ShapeBounding, 0, 0, pixmask, ShapeSet);
	XShapeCombineMask(DADisplay, DAIcon, ShapeBounding, 0, 0, pixmask, ShapeSet);

	DAShow();

}
