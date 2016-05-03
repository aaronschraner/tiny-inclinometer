#microcontroller information
MCU=attiny84
AVRDUDEMCU=t84
OBJ2HEX=avr-objcopy
AVRDUDE=avrdude
SPIDEV=/dev/spidev0.0
RSTPIN=3

CPPFILES=Shift595.cpp MMA7455.cpp main.cpp
CC=avr-g++
HFILES=AT84_common.h MMA7455.h Shift595.h
CFLAGS=-g -Os -std=c++11 -Wall -Wno-reorder -mcall-prologues -mmcu=$(MCU) 

###fuse configuration###
##use 8MHz internal clock
LFUSE=0xE2
##use 1MHz internal clock
#LFUSE=0x62
HFUSE=0xDF
EFUSE=0xFF
TARGET=inclinometer
BAUDRATE=10000

build: $(CPPFILES) $(HFILES)
	$(CC) $(CFLAGS) $(CPPFILES) -o $(TARGET).out
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET).out $(TARGET).hex

upload: build
	sudo gpio mode $(RSTPIN) out
	sudo gpio write $(RSTPIN) 0
	sudo $(AVRDUDE) -p $(AVRDUDEMCU) -P $(SPIDEV) -c linuxspi -b $(BAUDRATE) -U flash:w:$(TARGET).hex
	sudo gpio write $(RSTPIN) 1

noreset: build
	sudo $(AVRDUDE) -p $(AVRDUDEMCU) -P $(SPIDEV) -c linuxspi -b $(BAUDRATE) -U flash:w:$(TARGET).hex

fuse:
	sudo gpio mode $(RSTPIN) out
	sudo gpio write $(RSTPIN) 0
	sudo $(AVRDUDE) -p $(AVRDUDEMCU) -P $(SPIDEV) -c linuxspi -b $(BAUDRATE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m
	sudo gpio write $(RSTPIN) 1

reset:
	sudo gpio mode $(RSTPIN) out
	sudo gpio write $(RSTPIN) 0
	sudo gpio write $(RSTPIN) 1

clean:
	rm -fv $(TARGET).out $(TARGET).hex


