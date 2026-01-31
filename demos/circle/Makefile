TOP=../..
EXES=circle_test
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/addons/game_engine

circle_test: circle_test.o libv4p.a $(TOP)/addons/game_engine/libg4p.a
	$(CC) -o $@ circle_test.o -L$(TOP)/addons/game_engine -lv4p -lg4p $(CPPFLAGS)