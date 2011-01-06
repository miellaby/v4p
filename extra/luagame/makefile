TOP=../..
EXES=luagame
include $(TOP)/rules.mak
CPPFLAGS+= -I/usr/include/lua5.1

$(TOP)/bindings/lua/v4p4lua.o:
	$(MAKE) -C $(TOP) lua

luagame: luagame.o $(TOP)/bindings/lua/v4p4lua.o
	$(CC) $(CPPFLAGS) -o $@ -L$(TOP) $(TOP)/bindings/lua/v4p4lua.o -llua5.1 luagame.o -lv4p -lgm 
