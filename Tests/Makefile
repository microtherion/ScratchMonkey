undef :
	echo Please specify the chip to test

attiny85.isp : test8K.hex test512.hex
	../Tools/RunTest attiny85 stk500v2 test8K.hex test512.hex 62:D7:FF 72:D6:FE  
attiny84.isp : test8K.hex test512.hex
	../Tools/RunTest attiny84 stk500v2 test8K.hex test512.hex 62:DF:FF 72:D6:FE  
attiny4313.isp : test4K.hex test256.hex
	../Tools/RunTest attiny4313 stk500v2 test4K.hex test256.hex 62:9F:FE 72:DF:FF  
attiny861.isp : test8K.hex test512.hex
	../Tools/RunTest attiny861 stk500v2 test8K.hex test512.hex 62:DF:FF 63:DE:FE  
attiny1634.isp : test16K.hex test256.hex
	../Tools/RunTest attiny1634 stk500v2 test16K.hex test256.hex 62:DF:E7 72:DE:FF  
atmega328.isp : test28K.hex test1K.hex
	../Tools/RunTest atmega328 stk500v2 test28K.hex test1K.hex 62:D9:FF 72:D8:FE
arduino.isp : test28K.hex test1K.hex  
	../Tools/RunTest atmega328p stk500v2 test28K.hex test1K.hex FF:D6:FD FE:DE:FE
atmega1284.isp : test120K.hex test4K.hex
	../Tools/RunTest atmega1284p stk500v2 test120K.hex test4K.hex 62:99:FF 72:89:FD  

attiny85.hvsp : test8K.hex test512.hex
	../Tools/RunTest attiny85 stk500hvsp test8K.hex test512.hex 62:D7:FF 72:D6:FE  
digispark.hvsp : test8K.hex test512.hex
	../Tools/RunTest attiny85 stk500hvsp test8K.hex test512.hex E1:5D:FE 72:D6:FE  
attiny84.hvsp : test8K.hex test512.hex
	../Tools/RunTest attiny84 stk500hvsp test8K.hex test512.hex 62:DF:FF 72:D6:FE  

attiny4313.hvpp : test4K.hex test256.hex
	../Tools/RunTest attiny4313 stk500pp test4K.hex test256.hex 62:9F:FE 72:DF:FF  
attiny861.hvpp : test8K.hex test512.hex
	../Tools/RunTest attiny861 stk500pp test8K.hex test512.hex 62:DF:FF 63:DE:FE  
attiny1634.hvpp : test16K.hex test256.hex
	../Tools/RunTest attiny1634 stk500pp test16K.hex test256.hex 62:DF:E7 72:DE:FF  
atmega328.hvpp : test28K.hex test1K.hex
	../Tools/RunTest atmega328 stk500pp test28K.hex test1K.hex 62:D9:FF 72:D8:FE  
atmega1284.hvpp : test120K.hex test4K.hex
	../Tools/RunTest atmega1284p stk500pp test120K.hex test4K.hex 62:99:FF 72:89:FD  

test256.hex :
	../Tools/hexer 0 256 > $@
test512.hex :
	../Tools/hexer 0 512 > $@
test1K.hex :
	../Tools/hexer 0 0x0400 > $@
test4K.hex :
	../Tools/hexer 0 0x1000 > $@
test8K.hex :
	../Tools/hexer 0 0x2000 > $@
test16K.hex :
	../Tools/hexer 0 0x4000 > $@
test28K.hex :
	../Tools/hexer 0 0x7000 > $@
test120K.hex :
	../Tools/hexer 0 0x1E000 > $@
