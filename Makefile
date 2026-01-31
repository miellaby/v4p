TOP=.
LIBS=libv4p.a
ADDONS = v4pserial qfont game_engine luagame
DEMOS = square ved stars parent_test zoom_test nested_hexagon circle debug_circle qfont starfield_3d collision_test
include $(TOP)/rules.mak

#BACKEND=xlib

all: addons demos

libv4p.a: quickheap.o quicktable.o sortable.o lowmath.o v4p_color.o v4p_ll.o v4pi.o v4p.o
	$(AR) $(ARFLAGS) $@ $?

libv4pserial.a: v4pserial.o
	$(AR) $(ARFLAGS) $@ $?

libg4p.a: game_engine.o
	$(AR) $(ARFLAGS) $@ $?


# LUA binding

bindings/lua/v4p4lua.c: bindings/v4p.i v4p.h v4pi.h
	swig -I$(TOP)/v4pi/$(TARGET)/$(BACKEND) -I$(TOP)/v4pi/$(TARGET) -I$(TOP)/v4pi -I$(TOP) -lua -module v4p -o $@ bindings/v4p.i

bindings/lua/v4p4lua.o: bindings/lua/v4p4lua.c
bindings/lua/v4p4lua.o: CFLAGS += -I/usr/include/lua5.1 -fPIC

bindings/lua/v4p.so: libv4p.a bindings/lua/v4p4lua.o
	$(CC) -shared -fPIC -L/usr/lib/lua bindings/lua/v4p4lua.o $(CPPFLAGS) -o $@

lua: bindings/lua/v4p.so

clean.lua:
	-rm bindings/lua/v4p4lua.c bindings/lua/v4p.so

%.addon: $(LIBS)
	$(MAKE) -C addons/$(@:.addon=)

%.addonclean:
	$(MAKE) -C addons/$(@:.addonclean=) clean

addons: $(ADDONS:=.addon)

addonclean: $(ADDONS:=.addonclean)

demos: $(LIBS) addons $(DEMOS:=.demo)

democlean: $(DEMOS:=.democlean)

%.demo:
	$(MAKE) -C demos/$(@:.demo=)

%.democlean:
	$(MAKE) -C demos/$(@:.democlean=) clean

clean: clean.lua addonclean democlean
