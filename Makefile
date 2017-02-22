# dockapps which only require `make`
MAKE_DOCKAPPS = AlsaMixer.app \
	pclock \
	Temperature.app \
	wmacpi \
	wmbutton \
	wmcalc \
	wmcalclockkbd \
	wmckgmail \
	wmcliphist \
	wmcp \
	wmcpufreq

# dockapps which require `make -C Src`
MAKE_DOCKAPPS_SRC = wmCalClock \
	wmcalendar

# dockapps which require `make -C wmfoo` (where 'wmfoo' is the dockapp's name)
MAKE_DOCKAPPS_WMFOO = wmdonkeymon

# dockapps which use autotools
AUTOTOOLS_DOCKAPPS = libdockapp \
	wmacpiload \
	wmbatteries \
	wmbattery \
	wmbiff \
	wmcdplay \
	wmclock

ALL_DOCKAPPS = $(MAKE_DOCKAPPS) \
	$(AUTOTOOLS_DOCKAPPS) \
	$(MAKE_DOCKAPPS_SRC) \
	$(MAKE_DOCKAPPS_WMFOO)

#todo:
#  asbeats
#  ascd
#  cnslock
#  washerdryer
#  wmail
#  wmappkill
#  wmcapshare

all: $(ALL_DOCKAPPS)

$(MAKE_DOCKAPPS):
	cd $@ && make

$(MAKE_DOCKAPPS_SRC):
	cd $@ && make -C Src

$(MAKE_DOCKAPPS_WMFOO):
	cd $@ && make -C $@

$(AUTOTOOLS_DOCKAPPS):
	cd $@ && autoreconf -i && ./configure && make

washerdryer:
	cd $@ && make -C washerDryer

clean:
	git clean -fxd

.PHONY: $(ALL_DOCKAPPS)
