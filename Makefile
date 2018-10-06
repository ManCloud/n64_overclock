export MULTI=2_0X
%_CC: export LED_TYPE=CC
%_CC: export CFLAGS+= -DLED_TYPE=COMMON_CATHODE
%_3_0_CC: export MULTI=3_0X
%_3_0_CC: export CFLAGS+= -DMAXIMUM_MULTIPLIER=MULTIPLIER_3_0X

%_CA: export LED_TYPE=CA
%_CA: export CFLAGS+= -DLED_TYPE=COMMON_ANODE
%_3_0_CA: export MULTI=3_0X
%_3_0_CA: export CFLAGS+= -DMAXIMUM_MULTIPLIER=MULTIPLIER_3_0X

tiny13%: export CPU = attiny13
tiny13%: export AVRDUDE_CPU = t13

tiny45%: export CPU = attiny45
tiny45%: export AVRDUDE_CPU = t45
	
tiny2313%: export CPU = attiny2313
tiny2313%: export AVRDUDE_CPU = t2313

tiny%:
	make -f n64_overclock.mk all
	
.Phony: clean
clean:  
	make -f n64_overclock.mk clean
