vpath %.svg ../doc/img

%_isp.svg: %.dip %.isp
	../Tools/dipper $^ > ../doc/img/$@
%_hvsp.svg: %.dip %.hvsp
	../Tools/dipper $^ > ../doc/img/$@

SERIALS	= attiny85 attiny84
ISPS    = $(addsuffix _isp,$(SERIALS))
HVSPS   = $(addsuffix _hvsp,$(SERIALS))
SVGS    = $(addsuffix .svg,$(ISPS) $(HVSPS))

all: $(SVGS)

