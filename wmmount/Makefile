OBJS  = dockapp.o wmmount.o
LIBS = -lX11 -lXpm -lXext
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
INSTALL = install

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

wmmount: $(OBJS)
	$(CC) $(LDFLAGS) -o wmmount $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) wmmount

install: wmmount
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) wmmount $(DESTDIR)$(BINDIR)
