TOP=.
LIBS=libv4p.a
EXTRAS = v4pserial qfont game_engine luagame
DEMOS = square ved stars parent_test zoom_test
include $(TOP)/rules.mak
#BACKEND=xlib
libv4p.a: quickheap.o quicktable.o sortable.o lowmath.o v4p.o v4pi.o
	$(AR) $(ARFLAGS) $@ $?

libv4pserial.a: v4pserial.o
	$(AR) $(ARFLAGS) $@ $?

libgm.a: game_engine.o
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

clean: clean.lua
