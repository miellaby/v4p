TOP=../..
EXES=nested_hexagon
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine

nested_hexagon: nested_hexagon.o libv4p.a $(TOP)/extra/game_engine/libg4p.a
	$(CC) -o $@ nested_hexagon.o -L$(TOP)/extra/game_engine -lv4p -lg4p -lm $(CPPFLAGS)

clean:
	rm -f nested_hexagon *.o