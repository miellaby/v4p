TOP=../..
EXES=square
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine

square: square.o libv4p.a $(TOP)/extra/gamemachine/libgm.a
	$(CC) $(CPPFLAGS) -o $@ square.o -L$(TOP)/extra/gamemachine -lv4p -lgm 
