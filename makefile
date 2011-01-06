TOP=.
LIBS=libv4p.a libv4pserial.a libgm.a
DEMOS = square ved stars
include $(TOP)/rules.mak
#BACKEND=xlib
libv4p.a: quickheap.o quicktable.o sortable.o lowmath.o v4p.o v4pi.o
	$(AR) $(ARFLAGS) $@ $?

libv4pserial.a: v4pserial.o
	$(AR) $(ARFLAGS) $@ $?

libgm.a: gamemachine.o
	$(AR) $(ARFLAGS) $@ $?


# LUA binding

bindings/lua/v4p4lua.c: bindings/v4p.i v4p.h v4pi.h
	swig -I$(TOP)/$(TARGET)-$(BACKEND) -lua -module v4p -o $@ bindings/v4p.i

bindings/lua/v4p4lua.o: bindings/lua/v4p4lua.c
bindings/lua/v4p4lua.o: CFLAGS += -I/usr/include/lua5.1

bindings/lua/v4p.so: libv4p.a bindings/lua/v4p4lua.o
	$(CC) -shared $(CPPFLAGS)-L/usr/lib/lua bindings/lua/v4p4lua.o libv4p.a -o $@

lua: bindings/lua/v4p.so

clean.lua:
	-rm bindings/lua/v4p4lua.c bindings/lua/v4p.so

clean: clean.lua
