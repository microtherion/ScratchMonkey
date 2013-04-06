vpath %.svg ../doc/img

%_isp.svg: %.dip %.isp
	../Tools/dipper $^ > ../doc/img/$@
%_hvsp.svg: %.dip %.hvsp
	../Tools/dipper $^ > ../doc/img/$@
%_hvpp.svg: %.dip %.hvpp
	../Tools/dipper $^ > ../doc/img/$@

SERIALS	    = attiny85 attiny84
PARALLELS   = attiny861 attiny4313 attiny1634 atmega328 atmega1284 
ISPS        = $(addsuffix _isp,$(SERIALS) $(PARALLELS))
HVSPS       = $(addsuffix _hvsp,$(SERIALS))
HVPPS       = $(addsuffix _hvpp,$(PARALLELS))
SVGS        = $(addsuffix .svg,$(ISPS) $(HVSPS) $(HVPPS))

all: $(SVGS)

