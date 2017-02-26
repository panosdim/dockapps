# dockapps which only require `make`
MAKE_DOCKAPPS = \
	AlsaMixer.app \
	pclock \
	Temperature.app \
	wmacpi \
	wmbutton \
	wmcalc \
	wmcalclockkbd \
	wmckgmail \
	wmcliphist \
	wmcp \
	wmcpufreq \
	wmdots \
	wmfemon \
	wmfortune \
	wmhexaclock \
	wmifinfo \
	wmitime \
	wmix \
	wmkeys \
	wmload \
	wmMatrix \
	wmmenu \
	wmmixer \
	wmomikuzi \
	wmppp.app \
	wmsun \
	wmsupermon \
	wmtime \
	wmtunlo \
	wmymail

# dockapps which require `make -C src`
MAKE_DOCKAPPS_LOWERCASE_SRC = \
	wmjmail \
	wmmoonclock

# dockapps which require `make -C Src`
MAKE_DOCKAPPS_UPPERCASE_SRC = \
	wmCalClock \
	wmcalendar \
	wmWeather \
	wmxss

# dockapps which require `make -C wmfoo` (where 'wmfoo' is the dockapp's name)
MAKE_DOCKAPPS_WMFOO = \
	wmdonkeymon \
	wmmon \
	wmpop3 \
	wmSMPmon \
	wmtetris \
	wmtz

# dockapps which use autotools
AUTOTOOLS_DOCKAPPS = \
	libdockapp \
	wmacpiload \
	wmbatteries \
	wmbattery \
	wmbiff \
	wmcdplay \
	wmclock \
	wmfsm \
	wmget \
	wmgtemp \
	wmhdplop \
	wmifs \
	wmlongrun \
	wmmemload \
	wmmp3 \
	wmpager \
	wmshutdown \
	wmthemech \
	wmtop \
	wmtv \
	wmwifi

XMKMF_DOCKAPPS = \
	wmnet

ALL_DOCKAPPS = $(MAKE_DOCKAPPS) \
	$(AUTOTOOLS_DOCKAPPS) \
	$(MAKE_DOCKAPPS_LOWERCASE_SRC) \
	$(MAKE_DOCKAPPS_UPPERCASE_SRC) \
	$(MAKE_DOCKAPPS_WMFOO) \
	$(XMKMF_DOCKAPPS) \
	wmgrabimage \
	wmnet \
	wmpop3lb \
	wmsm.app \
	wmspaceweather \
	wmswallow \
	wmwork

#todo:
#  asbeats
#  ascd
#  cnslock
#  washerdryer
#  wmail
#  wmappkill
#  wmcapshare
#  wmfu
#  wmglobe
#  wmmixer-alsa
#  wmpower
#  wmradio
#  wmsmixer
#  wmtrash
#  wmwebcam
#  wmwlmon

all: $(ALL_DOCKAPPS)

$(MAKE_DOCKAPPS):
	cd $@ && make

$(MAKE_DOCKAPPS_LOWERCASE_SRC):
	cd $@ && make -C src

$(MAKE_DOCKAPPS_UPPERCASE_SRC):
	cd $@ && make -C Src

$(MAKE_DOCKAPPS_WMFOO):
	cd $@ && make -C $@

$(AUTOTOOLS_DOCKAPPS):
	cd $@ && autoreconf -i && ./configure && make

$(XMKMF_DOCKAPPS):
	cd $@ && xmkmf && make

washerdryer:
	cd $@ && make -C washerDryer

wmgrabimage:
	cd $@ && make -C wmGrabImage

wmpop3lb:
	cd $@ && make -C wmpop3

wmsm.app:
	cd $@ && make -C wmsm

wmspaceweather:
	cd $@ && make -C wmSpaceWeather

wmswallow:
	cd $@ && make xfree

wmwork:
	cd $@/src && autoreconf -i && ./configure && make

clean:
	git clean -fxd

.PHONY: $(ALL_DOCKAPPS)
