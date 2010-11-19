TARGETS = linux palmos
MODES = normal debug


MODE ?= normal
TARGET ?= linux

CC_DEFAULT=gcc
AR_DEFAULT=ar

CC_palmos=m68k-palmos-gcc
AR_palmos=m68k-palmos-ar
CCFLAGS_palmos= -g -fno_builtin -I/m68k_palmos/include
ARFLAGS_palmos= rcs

CCFLAGS_linux=
CCFLAGS_linux_debug= -g
ARFLAGS_linux= rvs

CCFLAGS_linux_fractal=
CCFLAGS_linux_fractal_debug= -g
ARFLAGS_linux_fractal= rvs

CC=$(CC_DEFAULT$(CC_$(TARGET))) $(CC_$(TARGET))
AR=$(AR_DEFAULT$(AR_$(TARGET))) $(AR_$(TARGET))
CPPFLAGS=$(CCFLAGS_$(TARGET)) $(CCFLAGS_$(TARGET)_$(MODE)) -I. -I$(TARGET)
ARFLAGS=$(ARFLAGS_$(TARGET)) $(ARFLAGS_$(TARGET)_$(MODE))

PROJECT=v4p
LIBS=libv4p.a libgm.a libv4pserial.a
EXES=
PREFIX=
PUBLIC_HEADERS=*.h

all: $(LIBS) $(EXES)

install: $(LIBS) $(EXES)
	-mkdir -p $(PREFIX)/usr/include/$(PROJECT)/
	cp $(LIBS) $(PREFIX)/usr/lib/
	cp $(PUBLIC_HEADERS) $(PREFIX)/usr/include/$(PROJECT)/

clean:
	-rm *.a *.o

libv4p.a: quickheap.o sortable.o lowmath.o v4p.o v4pi.o
	$(AR) $(ARFLAGS) $@ $?

libv4pserial.a: v4pserial.o
	$(AR) $(ARFLAGS) $@ $?

libgm.a: gamemachine.o
	$(AR) $(ARFLAGS) $@ $?

VPATH=.:$(TARGET)
