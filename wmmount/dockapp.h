/* dockapp - Provides Window Maker dock applet functions
 *
 * 17/06/99  Release 1.0 beta1
 * Copyright (C) 1999  Sam Hawker <shawkie@geocities.com>
 * This software comes with ABSOLUTELY NO WARRANTY
 * This software is free software, and you are welcome to redistribute it
 * under certain conditions
 * See the README file for details.
 */

#ifndef DOCKAPP_H
#define DOCKAPP_H


/* User defines
 */

#define DOCKAPP_WMAKER false
#define DOCKAPP_SHAPE  false
#define DOCKAPP_ASTEP  false
#define DOCKAPP_SIZE   64
#define DOCKAPP_ASSIZE 56
#define DOCKAPP_FG     "#DEDEDE"
#define DOCKAPP_BG     "#282828"
#define DOCKAPP_NAME   "wmmount"
#define DOCKAPP_CLASS  "WMMount"

/* End of user defines
 * AVOID MAKING MODIFICATIONS BEYOND THIS POINT
 */


/* Includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/types.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>


/* Type definitions
 */

#ifndef BOOL_TYPE
#define BOOL_TYPE
typedef enum {false=0, true=1} bool;
#endif


/* External variables
 */

extern int dockapp_size;

extern int dockapp_argc;
extern char * const *dockapp_argv;

extern Display *dockapp_d;

extern Atom dockapp_a_delwin;

extern Window dockapp_w_icon;
extern Window dockapp_w_main;
extern Window dockapp_w_root;
extern Window dockapp_w_active;

extern Pixmap dockapp_pm_tile;
extern Pixmap dockapp_pm_main;
extern Pixmap dockapp_pm_mask;

extern GC dockapp_gc_main;
extern GC dockapp_gc_clip;
extern GC dockapp_gc_mask;

extern unsigned long dockapp_color0;
extern unsigned long dockapp_color1;
extern unsigned long dockapp_color2;
extern unsigned long dockapp_color3;

extern XpmAttributes dockapp_xpmattr;

/* Functions and procedures
 */

/* Initialize (and parse command-line options)
 */
void dockapp_init(int argc, char * const *argv);

/* Free everything
 */
void dockapp_done();

/* Open X display
 */
int dockapp_open();

/* Close X display
 */
int dockapp_close();

/* Load color
 */
unsigned long dockapp_getcolor(const char *colorname);

/* Mix color
 */
unsigned long dockapp_mixcolor(const char *colorname1, int weight1, char *colorname2, int weight2);

/* Create pixmap
 */
void dockapp_createpixmap(char **data, char *buf, char *file, Pixmap *image, Pixmap *mask, int *width, int *height);

/* Exposure event handler
 */
void dockapp_expose();

/* Install clip/shape mask
 */
void dockapp_installmask();

#endif
