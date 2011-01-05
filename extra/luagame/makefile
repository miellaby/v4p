TOP=../..
EXES=luagame
include $(TOP)/rules.mak
CPPFLAGS+= -I/usr/include/lua5.1

luagame: luagame.o
	$(CC) $(CPPFLAGS) -o $@ luagame.o -L$(TOP) -llua5.1 -lv4p -lgm 
