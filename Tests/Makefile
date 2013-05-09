undef :
	echo Please specify the chip to test

attiny1634.hvpp : test16K.hex test256.hex
	../Tools/RunTest attiny1634 stk500pp test16K.hex test256.hex 62:DF:E7 72:DE:FF  

test256.hex :
	../Tools/hexer 0 256 > $@
test16K.hex :
	../Tools/hexer 0 0x4000 > $@
