TARGETS = linux palmos
BACKENDS = sdl xlib
MODES = normal debug
DEMOS = square ved

MODE ?= normal
TARGET ?= linux
BACKEND ?= sdl

PREFIX ?= /usr

CC_DEFAULT=gcc
AR_DEFAULT=ar

CC_palmos=m68k-palmos-gcc
AR_palmos=m68k-palmos-ar
PALMOS_INCLUDE=/m68k_palmos/include
CCFLAGS_palmos= -fno_builtin -I$(PALMOS_INCLUDE)
ARFLAGS_palmos= rcs
CCFLAGS_palmos_debug = -g

CCFLAGS_linux=
CCFLAGS_linux_debug= -pg -DDEBUG
ARFLAGS_linux= rvs

CCFLAGS_linux_sdl= -lSDL
CCFLAGS_linux_xlib= -lX11


CC=$(CC_DEFAULT$(CC_$(TARGET))) $(CC_$(TARGET))
AR=$(AR_DEFAULT$(AR_$(TARGET))) $(AR_$(TARGET))
CPPFLAGS=-L$(TOP) -I$(TOP) -I$(TOP)/$(TARGET)-$(BACKEND) $(CCFLAGS_$(TARGET))$(CCFLAGS_$(TARGET)_$(BACKEND)) $(CCFLAGS_$(TARGET)_$(MODE)) -I. 
ARFLAGS=$(ARFLAGS_$(TARGET)) $(ARFLAGS_$(TARGET)_$(MODE))

all: $(LIBS) $(EXES)

demo: all $(DEMOS:=.demo)

%.demo:
	$(MAKE) -C demo/$(@:.demo=)

install: $(LIBS) $(EXES)
ifneq ($(LIBS),)   
	-mkdir -p $(PREFIX)/lib/
	cp $(LIBS) $(PREFIX)/lib/
endif
ifneq ($(EXES),)   
	-mkdir -p $(PREFIX)/bin/
	cp $(EXES) $(PREFIX)/bin/
endif
ifneq ($(PUBLIC_HEADERS),)   
	-mkdir -p $(PREFIX)/include/$(PROJECT)/
	cp $(PUBLIC_HEADERS) $(PREFIX)/include/$(PROJECT)/
endif

clean:
	-rm *.o $(LIBS) $(EXES)

pkg: clean
	find -name .svn -prune -o -name '*~' -exec rm {} \;
VPATH=.:$(TOP)/$(TARGET)-$(BACKEND):$(TOP)
