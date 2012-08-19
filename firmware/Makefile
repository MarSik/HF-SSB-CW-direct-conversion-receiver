# Target device
MCU = atmega644p
F_CPU = 8000000
LFUSE = 0xE2
HFUSE = 0xDA
EFUSE = 0xFD

# Application files and targets
TARGET = radio

# Place -I options here
CINCS =
CXXINCS =

# Place extra C/CXX arguments here
CEXTRA =
CXXEXTRA =

# Source files for C compiler
SRC = radio.c i2c.c spi.c dds.c si570.c interface.c ir.c memory.c bandplan.c main.c lang.c lcd.c

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
	gcc -DTEST -o test_si570 -Wall test_si570.c si570.c
