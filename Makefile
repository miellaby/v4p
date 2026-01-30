TOP=.
LIBS=libv4p.a
EXTRAS = v4pserial qfont game_engine luagame
DEMOS = square ved stars parent_test zoom_test nested_hexagon circle debug_circle qfont starfield_3d
include $(TOP)/rules.mak
#BACKEND=xlib

all: extra demo

libv4p.a: quickheap.o quicktable.o sortable.o lowmath.o v4p.o v4pi.o
	$(AR) $(ARFLAGS) $@ $?

libv4pserial.a: v4pserial.o
	$(AR) $(ARFLAGS) $@ $?

libg4p.a: game_engine.o
	$(AR) $(ARFLAGS) $@ $?


# LUA binding

bindings/lua/v4p4lua.c: bindings/v4p.i v4p.h v4pi.h
	swig -I$(TOP)/$(TARGET)-$(BACKEND) -lua -module v4p -o $@ bindings/v4p.i

bindings/lua/v4p4lua.o: bindings/lua/v4p4lua.c
bindings/lua/v4p4lua.o: CFLAGS += -I/usr/include/lua5.1

bindings/lua/v4p.so: libv4p.a bindings/lua/v4p4lua.o
	$(CC) -shared -L/usr/lib/lua bindings/lua/v4p4lua.o $(CPPFLAGS) -o $@

lua: bindings/lua/v4p.so

clean.lua:
	-rm bindings/lua/v4p4lua.c bindings/lua/v4p.so

%.extra: $(LIBS)
	$(MAKE) -C extra/$(@:.extra=)

%.extraclean:
	$(MAKE) -C extra/$(@:.extraclean=) clean

extra: $(EXTRAS:=.extra)

extraclean: $(EXTRAS:=.extraclean)

demo: $(LIBS) extra $(DEMOS:=.demo)

democlean: $(DEMOS:=.democlean)

%.demo:
	$(MAKE) -C demo/$(@:.demo=)

%.democlean:
	$(MAKE) -C demo/$(@:.democlean=) clean

clean: clean.lua extraclean democlean
