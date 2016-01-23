vpath %.svg ../doc/img

%_prog_isp.svg: %.dip %_prog.isp isp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_prog_hvsp.svg: %.dip %_prog.hvsp hvsp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_prog_hvpp.svg: %.dip %_prog.hvpp hvpp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_prog_tpi.svg: %.dip %_prog.tpi tpi.proto
	../Tools/dipper $^ > ../doc/img/$@
%_isp.svg: %.dip %.isp isp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_hvsp.svg: %.dip %.hvsp hvsp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_hvpp.svg: %.dip %.hvpp hvpp.proto
	../Tools/dipper $^ > ../doc/img/$@
%_tpi.svg: %.dip %.tpi tpi.proto
	../Tools/dipper $^ > ../doc/img/$@

SERIALS	    = attiny85 attiny84
PARALLELS   = attiny861 attiny4313 attiny1634 atmega328 atmega1284
BOARDS	    = uno micro nano mega
ISPS        = $(addsuffix _isp,$(SERIALS) $(PARALLELS) $(BOARDS) $(addsuffix _prog,$(BOARDS)) icsp)
HVSPS       = $(addsuffix _hvsp,$(SERIALS) $(addsuffix _prog,$(BOARDS)) digispark)
HVPPS       = $(addsuffix _hvpp,$(PARALLELS) $(addsuffix _prog,$(BOARDS)))
TPIS	    = $(addsuffix _tpi,attiny10 $(addsuffix _prog,$(BOARDS)))
SVGS        = $(addsuffix .svg,$(ISPS) $(HVSPS) $(HVPPS) $(TPIS))

all: $(SVGS)
	../Tools/updateTargets ../doc/img/{uno,micro,nano,mega,digispark,icsp}* $(addprefix /,$(SVGS)) < ../doc/wizard.js >w && mv w ../doc/wizard.js
