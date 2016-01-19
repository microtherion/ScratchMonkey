vpath %.svg ../doc/img

%_prog_isp.svg: %.dip %_prog.isp isp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_prog_hvsp.svg: %.dip %_prog.hvsp hvsp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_prog_hvpp.svg: %.dip %_prog.hvpp hvpp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_isp.svg: %.dip %.isp isp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_hvsp.svg: %.dip %.hvsp hvsp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_hvpp.svg: %.dip %.hvpp hvpp.proto
	../Tools/dipper $^ > ../doc/img/$@

SERIALS	    = attiny85 attiny84
PARALLELS   = attiny861 attiny4313 attiny1634 atmega328 atmega1284
BOARDS	    = uno micro
ISPS        = $(addsuffix _isp,$(SERIALS) $(PARALLELS) $(BOARDS) $(addsuffix _prog,$(BOARDS)))
HVSPS       = $(addsuffix _hvsp,$(SERIALS) $(addsuffix _prog,$(BOARDS)))
HVPPS       = $(addsuffix _hvpp,$(PARALLELS) $(addsuffix _prog,$(BOARDS)))
SVGS        = $(addsuffix .svg,$(ISPS) $(HVSPS) $(HVPPS))

all: $(SVGS)
	../Tools/updateTargets ../doc/img/{uno,micro,digispark,icsp}* $(addprefix /,$(SVGS)) < ../doc/wizard.js >w && mv w ../doc/wizard.js
