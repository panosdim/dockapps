lib_LTLIBRARIES = libwmgeneral.la
libwmgeneral_la_SOURCES = wmgeneral.c misc.c list.c

AM_CFLAGS = $(X11_CFLAGS) $(Xext_CFLAGS) $(Xpm_CFLAGS)
LIBS += $(X11_LIBS) $(Xext_LIBS) $(Xpm_LIBS)

pkgincludedir = @includedir@/wmgeneral
pkginclude_HEADERS = wmgeneral.h misc.h list.h
