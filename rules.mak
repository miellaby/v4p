.PHONY: all clean demo democlean extra extraclean


TARGETS = linux palmos
BACKENDS = sdl xlib
MODES = normal debug testu

MODE ?= normal
TARGET ?= linux
BACKEND ?= sdl

PREFIX ?= /usr

CC_DEFAULT=gcc
AR_DEFAULT=ar

# PALMOS
CC_palmos=m68k-palmos-gcc
AR_palmos=m68k-palmos-ar
PALMOS_INCLUDE=/m68k_palmos/include
CCFLAGS_palmos= -fno_builtin -I$(PALMOS_INCLUDE)
ARFLAGS_palmos= rcs
CCFLAGS_palmos_debug = -g

# LINUX
CCFLAGS_linux=
CCFLAGS_linux_debug= -pg -DDEBUG
ARFLAGS_linux= rvs

# SDL
CCFLAGS_linux_sdl= -lSDL

# XLIB
CCFLAGS_linux_xlib= -lX11

ifneq ($(MODE),normal)
CPPFLAGS=$(subst testu,-DTESTU,$(subst debug,-DDEBUG,$(MODE)))
endif

CC=$(or $(CC_$(TARGET)),$(CC_DEFAULT))
AR=$(or $(AR_$(TARGET)),$(AR_DEFAULT))
CPPFLAGS=-L$(TOP) -I$(TOP) -I$(TOP)/$(TARGET)-$(BACKEND) $(CCFLAGS_$(TARGET))$(CCFLAGS_$(TARGET)_$(BACKEND)) $(CCFLAGS_$(TARGET)_$(MODE)) -I. 
ARFLAGS=$(ARFLAGS_$(TARGET)) $(ARFLAGS_$(TARGET)_$(MODE))

all: $(LIBS) $(EXES)

extra: all $(EXTRAS:=.extra)

extraclean: $(EXTRAS:=.extraclean)

%.extra:
	$(MAKE) -C extra/$(@:.extra=)

%.extraclean:
	$(MAKE) -C extra/$(@:.extraclean=) clean


demo: extra $(DEMOS:=.demo)

democlean: $(DEMOS:=.democlean)

%.demo:
	$(MAKE) -C demo/$(@:.demo=)

%.democlean:
	$(MAKE) -C demo/$(@:.democlean=) clean

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

clean: extraclean democlean
	-rm *.o $(LIBS) $(EXES)

pkg: clean
	find -name .svn -prune -o -name '*~' -exec rm {} \;
VPATH=./$(TARGET)-$(BACKEND):.:$(TOP)/$(TARGET)-$(BACKEND):$(TOP)
