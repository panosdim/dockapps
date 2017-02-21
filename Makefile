MAKE_DOCKAPPS = AlsaMixer.app \
	pclock \
	Temperature.app \
	wmacpi \
	wmbutton \
	wmcalc

AUTOTOOLS_DOCKAPPS = libdockapp \
	wmacpiload \
	wmbatteries \
	wmbattery \
	wmbiff

ALL_DOCKAPPS = $(MAKE_DOCKAPPS) $(AUTOTOOLS_DOCKAPPS)

#todo:
#  asbeats
#  ascd
#  cnslock
#  washerdryer
#  wmail
#  wmappkill

all: $(ALL_DOCKAPPS)

$(MAKE_DOCKAPPS):
	cd $@ && make

$(AUTOTOOLS_DOCKAPPS):
	cd $@ && autoreconf -fvi && ./configure && make

washerdryer:
	cd $@ && make -C washerDryer

clean:
	git clean -fxd

.PHONY: $(ALL_DOCKAPPS)
