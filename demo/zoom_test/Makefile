TOP=../..
EXES=zoom_test
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine

zoom_test: zoom_test.o libv4p.a $(TOP)/extra/game_engine/libgm.a
	$(CC) -o $@ zoom_test.o -L$(TOP)/extra/game_engine -lv4p -lgm $(CPPFLAGS)