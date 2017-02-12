### Edit here as you like :)
BINDIR = /usr/local/bin
############################

DEST = wmfortune

# Programs used to move the binary to the proper place.
INSTALL = /usr/bin/install
INSTALL_PROGRAM = ${INSTALL} -m 755
UNINSTALL_PROGRAM = rm -f

CC = gcc
OPTIMIZE = -O2
XLIBDIR = /usr/X11R6/lib
XLIB = -lXpm -lXext -lX11
OBJ = wmfortune.o ../libdockapp/src/damain.o ../libdockapp/src/dacolor.o \
	../libdockapp/src/dautil.o ../libdockapp/src/daargs.o \
	../libdockapp/src/dapixmap.o ../libdockapp/src/daevent.o \
	../libdockapp/src/dacallback.o

$(DEST) : $(OBJ)
	$(CC) $(OBJ) -L$(XLIBDIR) $(XLIB) -o $(DEST)

wmfortune.o: wmfortune.c
	$(CC) $(OPTIMIZE) $< -c

install:
	$(INSTALL_PROGRAM) ./$(DEST) $(BINDIR)/$(DEST)

uninstall:
	$(UNINSTALL_PROGRAM) $(BINDIR)/$(DEST)

clean:
	rm -rf $(DEST) $(OBJ) *~
