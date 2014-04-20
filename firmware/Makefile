# Target device
MCU = atmega644p
F_CPU = 8000000
LFUSE = 0xDE# crystal 8Mhz, 258CK+65ms
HFUSE = 0xDA
EFUSE = 0xFD

# Application files and targets
TARGET = tunerfw

# Place -I options here
CINCS =
CXXINCS =

# Place extra C/CXX arguments here
CEXTRA =
CXXEXTRA =

# Source files for C compiler
SRC = serial.c freq.c radio.c i2c.c spi.c tuner.c interface.c ir.c memory.c bandplan.c main.c lang.c lcd.c

# Source files for C++ compiler
CXXSRC =

# Source files in assembly
ASRC =

# Extra libraries
LDEXTRA = -lm

# This includes the rest of rules DO NOT DELETE
-include ~/Dropbox/EDA/Makefile.avr.inc
-include ~/.gEDA/Makefile.avr.inc


tests:
	gcc -DTEST -lm -o test_si570 -Wall test_si570.c si570.c freq.c
