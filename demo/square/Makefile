TOP=../..
EXES=square
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine

square: square.o libv4p.a $(TOP)/extra/game_engine/libgm.a
	$(CC) -o $@ square.o -L$(TOP)/extra/game_engine -lv4p -lgm $(CPPFLAGS)
