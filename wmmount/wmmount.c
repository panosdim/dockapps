/* wmmount - The Window Maker universal mount point
 *
 * 17/06/99  Release 1.0 beta2
 * Copyright (C) 1999  Sam Hawker <shawkie@geocities.com>
 * This software comes with ABSOLUTELY NO WARRANTY
 * This software is free software, and you are welcome to redistribute it
 * under certain conditions
 * See the README file for details.
 */


/* User defines
 */

/* Intervals
 *   INTERVAL  - Minimum interval unit in usecs
 *   INTERVAL1 - Update interval in multiples of def_interval
 *   INTERVAL2 - Button repeat interval in multiples of def_interval
 *   INTERVAL3 - Double-click interval in multiples of def_interval
 */
#define INTERVAL  250000
#define INTERVAL1 10
#define INTERVAL2 1
#define INTERVAL3 1

/* Maximum allowed mountpoints and icons
 */
#define MAXICONS   20
#define MAXMPOINTS 20

/* System-wide config file
 */
#define SYSCONFIG "/usr/X11R6/lib/X11/wmmount/system.wmmount"

/* Config file defaults
 */
#define MOUNTCMD  "/bin/mount %m"
#define UMOUNTCMD "/bin/umount %m"
#define OPENCMD   "/usr/X11R6/bin/nxterm -T '%n - %m' -e mc %m"
#define NAMEFONT  "-*-helvetica-bold-r-*-*-10-*-*-*-*-*-*-*"
#define USAGEFONT "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"


/* End of user defines
 *
 * -- AVOID MAKING MODIFICATIONS BEYOND THIS POINT --
 */


/* Includes
 */

#include "dockapp.h"

#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#ifdef __linux__
#include <mntent.h>
#include <sys/vfs.h>
#endif
#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/mount.h>
#endif

#include "xpm/main.xpm"


/* Variables
 */

Pixmap pm_main;
Pixmap pm_main_mask;

char *mountcmd=NULL;
char *umountcmd=NULL;
char *opencmd=NULL;
char *namefont=NULL;
char *usagefont=NULL;

Font f_name, f_usage;

Pixmap pm_icon[MAXICONS];
int icons=0;

struct mpinfo {
  char *mpoint;
  char *name;
  char *mountcmd;
  char *umountcmd;
  char *opencmd;
  int icon;
  int udisplay;
} mpoint[MAXMPOINTS];
int mpoints=0;

int namex, namey, usagex, usagey;

#define BUTTON1 (int)(1<<0)
#define BUTTON2 (int)(1<<1)
#define BUTTON3 (int)(1<<2)
#define DCLICK  (int)(1<<3)
#define PREV    BUTTON1
#define NEXT    BUTTON2

int state=0;

int select1=0;

int counter1=0;
int counter2=0;
int counter3=0;

bool mounted;
int usage;
char usagestr[8];


/* Functions and procedures
 */

int main(int argc, char **argv);

void init(int argc, char * const *argv);
void done();

void mainloop();

void buttonpress(XButtonEvent *xev);
void buttonrelease(XButtonEvent *xev);

int config_init();
void config_done();

void xpm_init();
void xpm_done();

void font_init();
void font_done();

void checkmount(bool forced);

void drawall();
void drawbtns(int btnmask);
void drawbtn(int x, int y, int w, int h, bool down);

void execcmd(char *cmd, char *path);

/* Implementation
 */

int main(int argc, char **argv) {
  /* Initialize
   */
  init(argc, (char * const *)argv);

  /* Initialize dockapp
   */
  dockapp_init(argc, (char * const *)argv);

  /* Open dockapp
   */
  if(!dockapp_open()) {
    
    /* Read config file
     */
    if(!config_init()) {

      /* Initialize xpms
       */
      xpm_init();
      
      /* Initialize fonts
       */
      font_init();

      /* Check mounts
       */
      checkmount(true);

      /* Draw everything
       */
      drawall();
      
      /* Execute main loop
       */
      mainloop();

      /* Free fonts
       */
      font_done();

      /* Free xpms
       */
      xpm_done();

      /* Free config file data
       */
      config_done();

    }

    /* Close dockapp
     */
    dockapp_close();
    
  }

  /* Free dockapp
   */
  dockapp_done();

  /* Free
   */
  done();

  /* Finish
   */
  return 0;
}

void init(int argc, char * const *argv) {
  /* Process command-line arguments
   */
  int option_index;
  int option_c;
  struct option long_options[] = {
    {"help", 0, NULL, 'h' },
    {NULL,   0, NULL, '\0'}
  };
  while(true) {
    option_index=0;
    option_c=getopt_long_only(argc, (char * const *)argv, "-h", long_options, &option_index);
    if(option_c==-1)
      break;
    if(option_c==1) {
      fprintf(stderr, "%s: unwanted argument -- '%s'\n", argv[0], optarg);
    }
    if(option_c>1) {
      switch(option_c) {
      case 'h':
	printf("wmmount - The Window Maker universal mount point\n");
        printf("\n");
	printf("17/06/99  Release 1.0 beta2\n");
        printf("Copyright (C) 1999  Sam Hawker <shawkie@geocities.com>\n");
        printf("This software comes with ABSOLUTELY NO WARRANTY\n");
        printf("This software is free software, and you are welcome to redistribute it\n");
        printf("under certain conditions\n");
	printf("See the README file for details.\n");
	printf("\n");
        printf("usage:\n");
        printf("\n");
	printf("   %s [options] [-- dockapp_options]\n", argv[0]);
        printf("\n");
        printf("options:\n");
        printf("\n");
	printf("   -h, --help           Show this help message\n");
        printf("\n");
        printf("dockapp options:\n");
        printf("\n");
        printf("   -h, --help           Show dockapp help message\n");
	exit(0);
      }
    }
  }
}

void done() {
  /* Free command-line option arguments
   */
}

void mainloop() {
  /* Do the main loop
   */
  XEvent xev;

  fd_set fds;
  struct timeval tv;

  XSelectInput(dockapp_d, dockapp_w_active, ButtonPressMask | ButtonReleaseMask | ExposureMask);
  XMapWindow(dockapp_d, dockapp_w_main);

  while(true) {
    while(XPending(dockapp_d)) {
      XNextEvent(dockapp_d, &xev);
      switch(xev.type) {
      case Expose:
	dockapp_expose();
	break;
      case ButtonPress:
        buttonpress(&xev.xbutton);
	break;
      case ButtonRelease:
	buttonrelease(&xev.xbutton);
	break;
      case ClientMessage:
	if(xev.xclient.data.l[0]==dockapp_a_delwin)
          return;
      }
    }
    FD_ZERO(&fds);
    FD_SET(ConnectionNumber(dockapp_d), &fds);
    tv.tv_sec=(long)INTERVAL/1000000L;
    tv.tv_usec=(long)INTERVAL%1000000L;
    if(!select(ConnectionNumber(dockapp_d)+1, &fds, NULL, NULL, &tv)) {
      counter1++;
      if(counter1>=INTERVAL1) {
	counter1=0;
	/* Update interval
	 */
	checkmount(false);
      }
      counter2++;
      if(counter2>=INTERVAL2) {
	counter2=0;
	/* Button repeat interval
	 */
	if(state & (PREV | NEXT)) {
	  if(state & PREV)
	    select1--;
	  else
	    select1++;
	  if(select1<0)
	    select1=mpoints-1;
	  if(select1>=mpoints)
	    select1=0;
	  checkmount(true);
	}
      }
      counter3++;
      if(counter3>=INTERVAL3) {
	counter3=0;
	/* Double-click interval/timeout
	 */
	state&=~DCLICK;
      }
    }
  }
}

void buttonpress(XButtonEvent *xev) {
  /* Process button press events
   */
  int x=xev->x-(dockapp_size/2-32);
  int y=xev->y-(dockapp_size/2-32);

  char *cmd;

  if(x>=33 && y>=47 && x<=45 && y<=57) {
    select1--;
    if(select1<0)
      select1=mpoints-1;
    counter2=0;
    state|=PREV;
    checkmount(true);
    return;
  }

  if(x>=46 && y>=47 && x<=58 && y<=57) {
    select1++;
    if(select1>mpoints-1)
      select1=0;
    counter2=0;
    state|=NEXT;
    checkmount(true);
    return;
  }

  if(x>=5 && y>=47 && x<=32 && y<=57) {
    if(mounted) {
      cmd=mpoint[select1].umountcmd;
      if(cmd==NULL)
	cmd=umountcmd;
      if(cmd==NULL)
	cmd=UMOUNTCMD;
    }
    else {
      cmd=mpoint[select1].mountcmd;
      if(cmd==NULL)
	cmd=mountcmd;
      if(cmd==NULL)
	cmd=MOUNTCMD;
    }
    execcmd(cmd, NULL);

    return;
  }

  if(x>=4 && y>=4 && x<=59 && y<=42) {
    if(mounted) {
      if(state & DCLICK) {

	cmd=mpoint[select1].opencmd;
	if(cmd==NULL)
	  cmd=opencmd;
        if(cmd==NULL)
	  cmd=OPENCMD;
        execcmd(cmd, NULL);

	state&=~DCLICK;
      }
      else {
	state|=DCLICK;
	counter3=0;
      }
    }
    return;
  }

}

void buttonrelease(XButtonEvent *xev) {
  /* Process button release events
   */
  state&=~(PREV | NEXT);
  drawbtns(PREV | NEXT);
  dockapp_expose();
}

int config_init() {
  /* Process config file
   */

  char *home;
  char *filename;

  FILE *f;

  char buf[256];

  int keynum1;
  char *keyarg1;
  static char *keys1[] = {
    "mountcmd=",
    "umountcmd=",
    "opencmd=",
    "namefont=",
    "usagefont=",
    "icon ",
    "mountpoint ",
    ""
  };
  
  int keynum2;
  char *keyarg2;
  static char *keys2[] = {
    "mountpoint ",
    "name=",
    "iconnumber=",
    "usagedisplay=",
    "mountcmd=",
    "umountcmd=",
    "opencmd=",
    ""
  };

  home=getenv("HOME");
  filename=(char *)malloc(strlen(home)+strlen("/.wmmount")+1);
  sprintf(filename, "%s/.wmmount", home);
  f=fopen(filename, "r");
  free(filename);
  if(f==NULL) {
    fprintf(stderr, "%s: User config file not found\n", dockapp_argv[0]);
    filename=SYSCONFIG;
    f=fopen(filename, "r");
    if(f==NULL) {
      fprintf(stderr, "%s: System config file not found\n", dockapp_argv[0]);
      return -1;
    }
  }

  while(1) {
    fgets(buf, 256, f);
    if(feof(f)!=0)
      break;
    buf[strlen(buf)-1]='\0';

    keynum1=0;
    while(1) {
      if(strncmp(buf, keys1[keynum1], strlen(keys1[keynum1]))==0)
	break;
      keynum1++;
    }

    keyarg1=buf+strlen(keys1[keynum1]);

    if(keynum1==0)
      mountcmd=strdup(keyarg1);
    if(keynum1==1)
      umountcmd=strdup(keyarg1);
    if(keynum1==2)
      opencmd=strdup(keyarg1);
    if(keynum1==3)
      namefont=strdup(keyarg1);
    if(keynum1==4)
      usagefont=strdup(keyarg1);
    if(keynum1==5) {
      if(icons<MAXICONS) {
	dockapp_createpixmap(NULL, NULL, keyarg1, &pm_icon[icons], NULL, NULL, NULL);
	icons++;
      }
    }
    if(keynum1==6) {
      while(mpoints<MAXMPOINTS && feof(f)==0) {
	mpoint[mpoints].mountcmd=NULL;
	mpoint[mpoints].umountcmd=NULL;
	mpoint[mpoints].opencmd=NULL;
	mpoint[mpoints].mpoint=strdup(keyarg1);

	while(1) {
	  fgets(buf, 256, f);
	  if(feof(f)!=0)
	    break;
	  buf[strlen(buf)-1]='\0';

	  keynum2=0;
	  while(1) {
	    if(strncmp(buf, keys2[keynum2], strlen(keys2[keynum2]))==0)
	      break;
	    keynum2++;
	  }

	  keyarg2=buf+strlen(keys2[keynum2]);

	  if(keynum2==0)
	    break;
	  if(keynum2==1)
	    mpoint[mpoints].name=strdup(keyarg2);
	  if(keynum2==2)
	    sscanf(keyarg2, "%i", &mpoint[mpoints].icon);
	  if(keynum2==3)
	    sscanf(keyarg2, "%i", &mpoint[mpoints].udisplay);
	  if(keynum2==4)
	    mpoint[mpoints].mountcmd=strdup(keyarg2);
	  if(keynum2==5)
	    mpoint[mpoints].umountcmd=strdup(keyarg2);
	  if(keynum2==6)
	    mpoint[mpoints].opencmd=strdup(keyarg2);
	}
	mpoints++;
      }
    }

  }

  fclose(f);

  if(mpoints==0) {
    fprintf(stderr, "%s: No mountpoints specified\n", dockapp_argv[0]);
    return -1;
  }

  return 0;
}

void config_done() {
  /* Free config file stuff
   */
  free(mountcmd);
  free(umountcmd);
  free(opencmd);
  free(namefont);
  free(usagefont);

  while(mpoints>0) {
    mpoints--;
    free(mpoint[mpoints].mpoint);
    free(mpoint[mpoints].name);
    free(mpoint[mpoints].mountcmd);
    free(mpoint[mpoints].umountcmd);
    free(mpoint[mpoints].opencmd);
  }
}

void xpm_init() {
  /* Create pixmaps from xpm data
   */
  dockapp_createpixmap(main_xpm, NULL, NULL, &pm_main, &pm_main_mask, NULL, NULL);

  XCopyArea(dockapp_d, pm_main_mask, dockapp_pm_mask, dockapp_gc_mask, 0, 0, 64, 64, 0, 0);
  dockapp_installmask();
}

void xpm_done() {
  /* Free xpms
   */

  /* Done automatically by X, so I won't bother
   */
}

void font_init() {
  /* Initialize fonts
   */
  XFontStruct *xfs;
  char *font;

  font=namefont;
  if(font==NULL)
    font=NAMEFONT;
  xfs=XLoadQueryFont(dockapp_d, font);

  f_name=xfs->fid;
  namex=6-xfs->min_bounds.lbearing;
  namey=5+xfs->ascent;
  usagey=namey;
  usagey+=xfs->descent;
  XFreeFontInfo(NULL, xfs, 1);

  font=usagefont;
  if(font==NULL)
    font=USAGEFONT;
  xfs=XLoadQueryFont(dockapp_d, USAGEFONT);
  
  f_usage=xfs->fid;
  usagex=6-xfs->min_bounds.lbearing;
  usagey+=xfs->ascent;
  XFreeFontInfo(NULL, xfs, 1);
}

void font_done() {
  /* Free fonts
   */

  /* Done automatically by X
   */
}

void checkmount(bool forced) {
  /* Check mount and usage
   */

  bool getmounted;
  int getusage, getavail;

  dev_t dev1, dev2;
  struct stat st;
  struct statfs sfs;
  char *buf;

  float value;
  char unit;

  getmounted=false;
  if(strcmp(mpoint[select1].mpoint, "/")==0)
    /* I think I can assume / is mounted.
     */
    getmounted=true;
  else {
    /* Check that the directory above the mount point is on a different device
     */
    buf=strdup(mpoint[select1].mpoint);

    stat(buf, &st);
    dev1=st.st_dev;

    if(buf[strlen(buf)-1]=='/')
      buf[strlen(buf)-1]='\0';
    *(strrchr(buf, '/')+1)='\0';

    stat(buf, &st);
    dev2=st.st_dev;

    if(memcmp(&dev1, &dev2, sizeof(dev_t))!=0)
      getmounted=true;

    free(buf);
  }

  if(getmounted && mpoint[select1].udisplay!=0) {
    statfs(mpoint[select1].mpoint, &sfs);
    getusage=(sfs.f_blocks-sfs.f_bfree);
    getavail=(sfs.f_bavail);
  }

  if(getmounted && (!mounted || (getusage!=usage) || forced)) {

    value=getusage;
    switch(mpoint[select1].udisplay) {
    case 1:
      value=getavail;
    case 2:
      value*=(float)sfs.f_bsize;

      value/=1024.0;
      unit='k';
      if(value>=999.5) {
	value/=1024.0;
	unit='M';
      }
      if(value>=999.5) {
	value/=1024.0;
	unit='G';
      }

      if(value<9.995) {
	sprintf(usagestr, "%1.02f%cB", value, unit);
	break;
      }
      if(value<99.95) {
	sprintf(usagestr, "%1.01f%cB", value, unit);
	break;
      }
      if(value<999.5) {
	sprintf(usagestr, "%1.0f%cB", value, unit);
	break;
      }
      strcpy(usagestr, "E big");
      break;
    case 3:
      if(getusage+getavail==0)
	strcpy(usagestr, "E div0");
      else
	sprintf(usagestr, "%.01f%%", getusage*100.0/(getusage+getavail));
      break;
    default:
      strcpy(usagestr, "");
    }
  }

  if((getmounted!=mounted) || (getmounted && getusage!=usage && mpoint[select1].udisplay!=0) || forced) {
    mounted=getmounted;
    usage=getusage;
    drawall();
    dockapp_expose();
  }

}

void drawall() {
  /* Draw everything
   */
  XCopyArea(dockapp_d, pm_main, dockapp_pm_main, dockapp_gc_clip, 0, 0, 64, 64, 0, 0);

  XCopyArea(dockapp_d, pm_icon[mpoint[select1].icon], dockapp_pm_main, dockapp_gc_main, 0, 0, 32, 10, 25, 30);

  XSetFont(dockapp_d, dockapp_gc_main, f_name);
  XSetForeground(dockapp_d, dockapp_gc_main, dockapp_color0);
  XDrawString(dockapp_d, dockapp_pm_main, dockapp_gc_main, namex, namey, (char *)mpoint[select1].name, strlen(mpoint[select1].name));

  if(mounted) {
    XSetFont(dockapp_d, dockapp_gc_main, f_usage);
    XSetForeground(dockapp_d, dockapp_gc_main, dockapp_color0);
    XDrawString(dockapp_d, dockapp_pm_main, dockapp_gc_main, usagex, usagey, (char *)usagestr, strlen(usagestr));
  }

  drawbtns(BUTTON1 | BUTTON2 | BUTTON3);
}

void drawbtns(int btnmask) {
  /* Draw buttons
   */
  if(btnmask & BUTTON1)
    drawbtn(33, 47, 13, 11, (state & BUTTON1));
  if(btnmask & BUTTON2)
    drawbtn(46, 47, 13, 11, (state & BUTTON2));
  if(btnmask & BUTTON3)
    drawbtn(5, 47, 28, 11, mounted);
}

void drawbtn(int x, int y, int w, int h, bool down) {
  /* Draw single button
   */
  if(down) {
    XCopyArea(dockapp_d, pm_main, dockapp_pm_main, dockapp_gc_main, x, y, 1, h-1, x+w-1, y+1);
    XCopyArea(dockapp_d, pm_main, dockapp_pm_main, dockapp_gc_main, x+w-1, y+1, 1, h-1, x, y);
    XCopyArea(dockapp_d, pm_main, dockapp_pm_main, dockapp_gc_main, x, y, w-1, 1, x+1, y+h-1);
    XCopyArea(dockapp_d, pm_main, dockapp_pm_main, dockapp_gc_main, x+1, y+h-1, w-1, 1, x, y);
  }
  else
    XCopyArea(dockapp_d, pm_main, dockapp_pm_main, dockapp_gc_main, x, y, w, h, x, y);
}

void execcmd(char *cmd, char *path) {
  /* Execute external command (without using /bin/sh)
   */

  /* This has got quite complicated now
   *
   * It splits the command-line into its arguments, performs substitutions and
   * then executes the command
   *
   * It doesn't wait for a return and children are reaped using SIGCHLD
   */

  int argc=1;
  char **argv=NULL;

  int i, j;
  char c;

  int buflen=strlen(cmd)+1;
  char *buf=(char *)malloc(buflen);

  bool quoted=false;
  char quotec;

  char *subst;

  int pid;

  signal(SIGCHLD, SIG_IGN);

  j=0;
  for(i=0;i<strlen(cmd);i++) {
    c=cmd[i];
    if(!quoted && (c=='\'' || c=='\"')) {
      quoted=true;
      quotec=c;
      continue;
    }
    if(quoted && c==quotec) {
      quoted=false;
      continue;
    }
    if(c=='\\') {
      i++;
      if(i<strlen(cmd))
	c=cmd[i];
      else
	break;
    }
    if(c=='%') {
      i++;
      if(i<strlen(cmd))
	c=cmd[i];
      else
	break;
      subst=NULL;
      if(c=='m')
	subst=mpoint[select1].mpoint;
      if(c=='n')
	subst=mpoint[select1].name;
      if(subst!=NULL) {
	buflen+=strlen(subst)-2;
	buf=(char *)realloc(buf, buflen);
	strcpy(buf+j, subst);
	j+=strlen(subst);
	continue;
      }
    }
    if(!quoted && c==' ') {
      c='\0';
      argc++;
    }
    buf[j]=c;
    j++;
  }
  buf[j]='\0';

  argv=(char **)malloc((argc+1)*sizeof(char *));

  j=0;
  for(i=0;i<argc;i++) {
    argv[i]=buf+j;
    j+=strlen(buf+j)+1;
  }
  argv[argc]=NULL;

  pid=fork();
  if(pid==0) {
    if(path!=NULL)
      chdir(path);
    execv(argv[0], argv);
    free(argv);
    free(buf);
    exit(127);
  }
}
