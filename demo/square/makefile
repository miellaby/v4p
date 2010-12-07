TARGET ?= linux
MODE ?= normal

CC_palmos=m68k-palmos-gcc
AR_palmos=m68k-palmos-ar
CCFLAGS_palmos= -g -fno_builtin -I/m68k_palmos/include
ARFLAGS_palmos= rcs

CC_linux=gcc
AR_linux=ar
ARFLAGS_linux= rsv
CCFLAGS_linux=
CCFLAGS_linux_debug= -pg

CC=$(CC_$(TARGET))
AR=$(AR_$(TARGET))
CPPFLAGS=$(CCFLAGS_$(TARGET)) $(CCFLAGS_$(TARGET)_$(MODE)) -I. -I../../$(TARGET) -I../..
ARFLAGS=$(ARFLAGS_$(TARGET)) $(ARFLAGS_$(TARGET)_$(MODE))

all: square

clean:
	-rm *.a *.o square

square: main.o ../../libv4p.a
	gcc $(CPPFLAGS) -o $@ main.o -L../.. -lSDL -lv4p -lgm 
