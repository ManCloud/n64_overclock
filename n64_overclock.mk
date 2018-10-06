CC=avr-gcc
AS=$(CC)
LD=$(CC)

PROJECT=n64_overclock
TARGET=$(PROJECT)_$(AVRDUDE_CPU)_$(MULTI)_$(LED_TYPE)
VERSION=1.0

UISP=uisp -dprog=stk500 -dpart=attiny13 -dserial=/dev/avr
CFLAGS+=-Wall -Werror -mmcu=$(CPU) -DF_CPU=128000L -Os -DVERSION_STR=\"$(VERSION)\" -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields
LDFLAGS=-mmcu=$(CPU)
AVRDUDE=avrdude

PROGRAMMER=usbtiny

OBJS=$(PROJECT).o

all: $(TARGET).hex

.Phony: clean %.o
clean: 
	-rm -f $(PROJECT)*.elf $(PROJECT)*.hex $(PROJECT)*.o

$(TARGET).elf: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@
	
$(TARGET).hex: $(TARGET).elf
	avr-objcopy -j .data -j .text -O ihex $< $@
	avr-size $<

#
# FUSE low byte
#
# 7: SPIEN    -> 0
# 6: EESAVE   -> 1
# 5: WDTON    -> 1
# 4: CKDIV8   -> 1
# 3: SUT1     -> 1
# 2: SUT0     -> 0
# 1: CKSEL1   -> 1
# 0: CKSEL0   -> 1
#
# FUSE high byte
#
# 7: RESERVED  -> 1
# 6: RESERVED  -> 1
# 5: RESERVED  -> 1
# 4: SELFPRGEN -> 1
# 3: DWEN      -> 1
# 2: BODLEVEL1 -> 1
# 1: BODLEVEL0 -> 1
# 0: RSTDISBL  -> 0
#

fuse:
	$(AVRDUDE) -p $(AVRDUDE_CPU) -P usb -c $(PROGRAMMER) -Uhfuse:w:0xfe:m -Ulfuse:w:0x7b:m -B 64.0 -F

flash: $(HEXFILE)
	$(AVRDUDE) -p $(AVRDUDE_CPU) -P usb -c $(PROGRAMMER) -Uflash:w:$(PROJECT).hex -B 64.0 -F

reset:
	$(AVRDUDE) -p $(AVRDUDE_CPU) -P usb -c $(PROGRAMMER) -B 64.0 -F

%.o: %.c FORCE
	$(CC) $(CFLAGS) -c $<
	
FORCE: