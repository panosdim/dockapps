PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man6
INSTALL = install
LIBS   = -lXpm -lXext -lX11 -ldockapp
OBJS =  wmtictactoe.o

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $*.o

all: wmtictactoe

wmtictactoe: $(OBJS)
	$(CC) $(LDFLAGS) -o wmtictactoe $^ $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm -rf $$i;\
	done
	rm -rf wmtictactoe core *~

install:
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) wmtictactoe $(DESTDIR)$(BINDIR)
	$(INSTALL) -d $(DESTDIR)$(MANDIR)
	$(INSTALL) -m 644 wmtictactoe.6 $(DESTDIR)$(MANDIR)
