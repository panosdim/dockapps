/* dockapp - Provides Window Maker dock applet functions
 *
 * 17/06/99  Release 1.0 beta1
 * Copyright (C) 1999  Sam Hawker <shawkie@geocities.com>
 * This software comes with ABSOLUTELY NO WARRANTY
 * This software is free software, and you are welcome to redistribute it
 * under certain conditions
 * See the README file for details.
 */


/* Includes
 */

#include "dockapp.h"

#include "xpm/dockapp_tile.xpm"


/* Variables
 */

bool dockapp_wmaker=DOCKAPP_WMAKER;
bool dockapp_shape=DOCKAPP_SHAPE;
bool dockapp_astep=DOCKAPP_ASTEP;
int dockapp_size;

char *arg_fg=NULL;
char *arg_bg=NULL;
char *arg_pos=NULL;
char *arg_dpy=NULL;

int dockapp_argc;
char * const *dockapp_argv;

Display *dockapp_d;

Atom dockapp_a_delwin;

Window dockapp_w_icon;
Window dockapp_w_main;
Window dockapp_w_root;
Window dockapp_w_active;

Pixmap dockapp_pm_tile;
Pixmap dockapp_pm_main;
Pixmap dockapp_pm_mask;

GC dockapp_gc_main;
GC dockapp_gc_clip;
GC dockapp_gc_mask;

unsigned long dockapp_color0;
unsigned long dockapp_color1;
unsigned long dockapp_color2;
unsigned long dockapp_color3;

XpmAttributes dockapp_xpmattr;
XpmColorSymbol dockapp_xpmcsym[4]={{"dockapp_color0", NULL, 0},
				   {"dockapp_color1", NULL, 0},
				   {"dockapp_color2", NULL, 0},
				   {"dockapp_color3", NULL, 0}};


/* Functions and procedures
 */

void dockapp_createwin(Window *win, int x, int y);


/* Implementation
 */

void dockapp_init(int argc, char * const *argv) {
  int option_index;
  int option_c;
  struct option long_options[]={
    {"help",        0, NULL, 'h' },
    {"windowmaker", 0, NULL, 'w' },
    {"shape",       0, NULL, 's' },
    {"afterstep",   0, NULL, 'a' },
    {"foreground",  1, NULL, 'f' },
    {"background",  1, NULL, 'b' },
    {"position",    1, NULL, '\0'},
    {"display",     1, NULL, '\0'},
    {NULL,          0, NULL, '\0'}
  };
  dockapp_argc=argc;
  dockapp_argv=argv;
  while(true) {
    option_index=0;
    option_c=getopt_long_only(argc, argv, "-hwsaf:b:", long_options, &option_index);
    if(option_c==-1)
      break;
    if(option_c==0) {
      switch(option_index) {
      case 6:
	arg_pos=strdup(optarg);
	break;
      case 7:
	arg_dpy=strdup(optarg);
	break;
      }
    }
    if(option_c==1) {
      fprintf(stderr, "%s: unwanted argument -- '%s'\n", argv[0], optarg);
      break;
    }
    if(option_c>1) {
      switch(option_c) {
      case 'h':
	printf("dockapp - Provides Window Maker dock applet functions\n");
	printf("\n");
	printf("17/06/99  Release 1.0 beta1\n");
	printf("Copyright (C) 1999  Sam Hawker <shawkie@geocities.com>\n");
	printf("This software comes with ABSOLUTELY NO WARRANTY\n");
	printf("This software is free software, and you are welcome to redistribute it\n");
	printf("under certain conditions\n");
	printf("See the README file for details.\n");
	printf("\n");
	printf("dockapp options:\n");
	printf("\n");
	printf("   -h, --help                Show this help message\n");
	printf("   -w, --windowmaker         Use a withdrawn window (for Window Maker Dock)\n");
	printf("   -s, --shape               Use a shaped window\n");
	printf("   -a, --afterstep           Use a smaller window (for AfterStep Wharf)\n");
	printf("   -f, --foreground color    Set foreground color\n");
        printf("   -b, --background color    Set background color\n");
	printf("       --position position   Set window position (see X manual pages)\n");
	printf("       --display display     Set X display (see X manual pages)\n");
	exit(0);
      case 'w':
	dockapp_wmaker=!dockapp_wmaker;
	break;
      case 's':
	dockapp_shape=!dockapp_shape;
	break;
      case 'a':
	dockapp_astep=!dockapp_astep;
	break;
      case 'f':
	arg_fg=strdup(optarg);
        break;
      case 'b':
	arg_bg=strdup(optarg);
	break;
      }
    }
  }
}

void dockapp_done() {
  if(arg_fg!=NULL)
    free(arg_fg);
  if(arg_bg!=NULL)
    free(arg_bg);
  if(arg_pos!=NULL)
    free(arg_pos);
  if(arg_dpy!=NULL)
    free(arg_dpy);
}

int dockapp_open() {
  XWMHints wmhints;
  XSizeHints shints;

  XpmAttributes xpmattr;

  XGCValues gcv;
  unsigned long gcm;

  char *dpy="";
  char *pos="";
  char *fg=DOCKAPP_FG;
  char *bg=DOCKAPP_BG;

  if(arg_dpy!=NULL)
    dpy=arg_dpy;
  if(arg_pos!=NULL)
    pos=arg_pos;
  if(arg_fg!=NULL)
    fg=arg_fg;
  if(arg_bg!=NULL)
    bg=arg_bg;

  if(dockapp_astep)
    dockapp_size=DOCKAPP_ASSIZE;
  else
    dockapp_size=DOCKAPP_SIZE;

  if((dockapp_d=XOpenDisplay(dpy))==NULL) {
    fprintf(stderr, "%s: Unable to open display '%s'\n", dockapp_argv[0], XDisplayName(dpy));
    return -1;
  }

  dockapp_a_delwin=XInternAtom(dockapp_d, "WM_DELETE_WINDOW", false);

  dockapp_w_root=DefaultRootWindow(dockapp_d);

  shints.x=0;
  shints.y=0;
  shints.flags=0;
  if(XWMGeometry(dockapp_d, DefaultScreen(dockapp_d), pos, NULL, 1, &shints, &shints.x, &shints.y, &shints.width, &shints.height, &shints.win_gravity) & (XValue | YValue))
    shints.flags=PMinSize | PMaxSize | USPosition;
  else
    shints.flags=PMinSize | PMaxSize;
  shints.min_width=dockapp_size;
  shints.min_height=dockapp_size;
  shints.max_width=dockapp_size;
  shints.max_height=dockapp_size;

  dockapp_createwin(&dockapp_w_main, shints.x, shints.y);

  if(dockapp_wmaker | dockapp_astep)
    shints.flags |= USPosition;

  if(dockapp_wmaker) {
    wmhints.initial_state=WithdrawnState;
    wmhints.flags=WindowGroupHint | StateHint | IconWindowHint;
    dockapp_createwin(&dockapp_w_icon, shints.x, shints.y);
    dockapp_w_active=dockapp_w_icon;
    wmhints.icon_window=dockapp_w_icon;
  }
  else {
    wmhints.initial_state=NormalState;
    wmhints.flags=WindowGroupHint | StateHint;
    dockapp_w_active=dockapp_w_main;
  }
  wmhints.window_group=dockapp_w_main;

  XSetWMHints(dockapp_d, dockapp_w_main, &wmhints);
  XSetWMNormalHints(dockapp_d, dockapp_w_main, &shints);
  XSetCommand(dockapp_d, dockapp_w_main, (char **)dockapp_argv, dockapp_argc);
  XStoreName(dockapp_d, dockapp_w_main, DOCKAPP_NAME);
  XSetIconName(dockapp_d, dockapp_w_main, DOCKAPP_NAME);
  XSetWMProtocols(dockapp_d, dockapp_w_active, &dockapp_a_delwin, 1);

  xpmattr.exactColors=false;
  xpmattr.closeness=40000;
  xpmattr.valuemask=XpmExactColors | XpmCloseness;
  XpmCreatePixmapFromData(dockapp_d, dockapp_w_root, dockapp_tile_xpm, &dockapp_pm_tile, NULL, &xpmattr);
  dockapp_pm_main=XCreatePixmap(dockapp_d, dockapp_w_root, 64, 64, DefaultDepth(dockapp_d, DefaultScreen(dockapp_d)));
  dockapp_pm_mask=XCreatePixmap(dockapp_d, dockapp_w_root, 64, 64, 1);

  gcm=GCGraphicsExposures;
  gcv.graphics_exposures=false;
  dockapp_gc_main=XCreateGC(dockapp_d, dockapp_w_root, gcm, &gcv);
  dockapp_gc_clip=XCreateGC(dockapp_d, dockapp_w_root, gcm, &gcv);
  dockapp_gc_mask=XCreateGC(dockapp_d, dockapp_pm_mask, gcm, &gcv);

  dockapp_color0=dockapp_mixcolor(fg, 100, bg, 0);
  dockapp_color1=dockapp_mixcolor(fg, 0, bg, 100);
  dockapp_color2=dockapp_mixcolor(fg, 60, bg, 40);
  dockapp_color3=dockapp_mixcolor(fg, 25, bg, 75);

  dockapp_xpmcsym[0].pixel=dockapp_color0;
  dockapp_xpmcsym[1].pixel=dockapp_color1;
  dockapp_xpmcsym[2].pixel=dockapp_color2;
  dockapp_xpmcsym[3].pixel=dockapp_color3;

  dockapp_xpmattr.numsymbols=4;
  dockapp_xpmattr.colorsymbols=dockapp_xpmcsym;
  dockapp_xpmattr.exactColors=false;
  dockapp_xpmattr.closeness=40000;
  dockapp_xpmattr.valuemask=XpmColorSymbols | XpmExactColors | XpmCloseness | XpmSize;

  return 0;
}

int dockapp_close() {
  XCloseDisplay(dockapp_d);
  dockapp_d=NULL;
}

void dockapp_createwin(Window *win, int x, int y) {
  XClassHint chint;
  *win=XCreateSimpleWindow(dockapp_d, dockapp_w_root, x, y, dockapp_size, dockapp_size, 0, 0, 0);
  chint.res_name=DOCKAPP_NAME;
  chint.res_class=DOCKAPP_CLASS;
  XSetClassHint(dockapp_d, *win, &chint);
}

unsigned long dockapp_getcolor(const char *colorname) {
  XColor color;
  XWindowAttributes wattr;
  XGetWindowAttributes(dockapp_d, dockapp_w_root, &wattr);
  color.pixel=0;
  XParseColor(dockapp_d, wattr.colormap, colorname, &color);
  color.flags=DoRed | DoGreen | DoBlue;
  XAllocColor(dockapp_d, wattr.colormap, &color);
  return color.pixel;
}

unsigned long dockapp_mixcolor(const char *colorname1, int weight1, char *colorname2, int weight2) {
  XColor color1, color2, color;
  XWindowAttributes wattr;
  XGetWindowAttributes(dockapp_d, dockapp_w_root, &wattr);
  XParseColor(dockapp_d, wattr.colormap, colorname1, &color1);
  XParseColor(dockapp_d, wattr.colormap, colorname2, &color2);
  color.pixel=0;
  color.red=(color1.red*weight1+color2.red*weight2)/(weight1+weight2);
  color.green=(color1.green*weight1+color2.green*weight2)/(weight1+weight2);
  color.blue=(color1.blue*weight1+color2.blue*weight2)/(weight1+weight2);
  color.flags=DoRed | DoGreen | DoBlue;
  XAllocColor(dockapp_d, wattr.colormap, &color);
  return color.pixel;
}

void dockapp_createpixmap(char **data, char *buf, char *file, Pixmap *image, Pixmap *mask, int *width, int *height) {
  if(data!=NULL)
    XpmCreatePixmapFromData(dockapp_d, dockapp_w_root, data, image, mask, &dockapp_xpmattr);
  else {
    if(buf!=NULL)
      XpmCreatePixmapFromBuffer(dockapp_d, dockapp_w_root, buf, image, mask, &dockapp_xpmattr);
    else
      XpmReadFileToPixmap(dockapp_d, dockapp_w_root, file, image, mask, &dockapp_xpmattr);
  }

  if(width!=NULL)
    *width=dockapp_xpmattr.width;
  if(height!=NULL)
    *height=dockapp_xpmattr.height;
}

void dockapp_expose() {
  XEvent xev;
  XCopyArea(dockapp_d, dockapp_pm_main, dockapp_w_active, dockapp_gc_main, 0, 0, 64, 64, dockapp_size/2-32, dockapp_size/2-32);
  while(XCheckTypedEvent(dockapp_d, Expose, &xev));
}

void dockapp_installmask() {
  if((dockapp_wmaker || dockapp_shape || dockapp_astep))
    XShapeCombineMask(dockapp_d, dockapp_w_active, ShapeBounding, dockapp_size/2-32, dockapp_size/2-32, dockapp_pm_mask, ShapeSet);
  else {
    XSetClipMask(dockapp_d, dockapp_gc_clip, dockapp_pm_mask);
    XCopyArea(dockapp_d, dockapp_pm_tile, dockapp_pm_main, dockapp_gc_main, 0, 0, 64, 64, dockapp_size/2-32, dockapp_size/2-32);
  }
}
