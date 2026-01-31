TOP=../..
EXES=luagame
include $(TOP)/rules.mak
CPPFLAGS+= -I/usr/include/lua5.1 -I$(TOP)/addons/game_engine

$(TOP)/bindings/lua/v4p4lua.o:
	$(MAKE) -C $(TOP) lua

luagame: luagame.o $(TOP)/bindings/lua/v4p4lua.o
	$(CC) -o $@ \
	-L$(TOP)/addons/game_engine \
	$(TOP)/bindings/lua/v4p4lua.o \
	-llua5.1 luagame.o -lv4p -lg4p $(CPPFLAGS) 
