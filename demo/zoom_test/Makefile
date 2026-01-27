TOP=../..
EXES=zoom_test
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine

zoom_test: zoom_test.o libv4p.a $(TOP)/extra/gamemachine/libgm.a
	$(CC) -o $@ zoom_test.o -L$(TOP)/extra/gamemachine -lv4p -lgm $(CPPFLAGS)