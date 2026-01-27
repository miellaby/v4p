TOP=../..
EXES=parent_test
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine

parent_test: parent_test.o libv4p.a $(TOP)/extra/game_engine/libgm.a
	$(CC) -o $@ parent_test.o -L$(TOP)/extra/game_engine -lv4p -lgm $(CPPFLAGS)

clean:
	rm -f parent_test *.o