TOP=../..
EXES=palette
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine

palette: palette.o libv4p.a $(TOP)/extra/gamemachine/libgm.a
	$(CC) -o $@ palette.o -L$(TOP)/extra/gamemachine -lv4p -lgm $(CPPFLAGS)
