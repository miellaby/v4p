TARGETS = linux palmos
BACKENDS = sdl xlib
MODES = normal debug
DEMOS = square ved

MODE ?= normal
TARGET ?= linux
BACKEND ?= sdl

CC_DEFAULT=gcc
AR_DEFAULT=ar

CC_palmos=m68k-palmos-gcc
AR_palmos=m68k-palmos-ar
PALMOS_INCLUDE=/m68k_palmos/include
CCFLAGS_palmos= -fno_builtin -I$(PALMOS_INCLUDE)
ARFLAGS_palmos= rcs
CCFLAGS_palmos_debug = -g

CCFLAGS_linux=
CCFLAGS_linux_debug= -pg
ARFLAGS_linux= rvs

CC=$(CC_DEFAULT$(CC_$(TARGET))) $(CC_$(TARGET))
AR=$(AR_DEFAULT$(AR_$(TARGET))) $(AR_$(TARGET))
CPPFLAGS=$(CCFLAGS_$(TARGET)) $(CCFLAGS_$(TARGET)_$(MODE)) -I. -I$(TARGET)-$(BACKEND)
ARFLAGS=$(ARFLAGS_$(TARGET)) $(ARFLAGS_$(TARGET)_$(MODE))

PROJECT=v4p
LIBS=libv4p.a libgm.a libv4pserial.a
EXES=
PREFIX=
PUBLIC_HEADERS=*.h

all: $(LIBS) $(EXES)

demo: all $(DEMOS:=.demo)

%.demo:
	$(MAKE) -C demo/$(@:.demo=)
   
install: $(LIBS) $(EXES)
	-mkdir -p $(PREFIX)/usr/include/$(PROJECT)/
	cp $(LIBS) $(PREFIX)/usr/lib/
	cp $(PUBLIC_HEADERS) $(PREFIX)/usr/include/$(PROJECT)/

clean:
	-rm *.a *.o

libv4p.a: quickheap.o quicktable.o sortable.o lowmath.o v4p.o v4pi.o
	$(AR) $(ARFLAGS) $@ $?

libv4pserial.a: v4pserial.o
	$(AR) $(ARFLAGS) $@ $?

libgm.a: gamemachine.o
	$(AR) $(ARFLAGS) $@ $?

VPATH=.:$(TARGET)-$(BACKEND)
