TOP=../..
EXES=qfont_demo
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine -I$(TOP)/extra/qfont

qfont_demo: qfont_demo.o libv4p.a $(TOP)/extra/game_engine/libg4p.a $(TOP)/extra/qfont/qfont.o
	$(CC) -o $@ qfont_demo.o $(TOP)/extra/qfont/qfont.o \
	  -L$(TOP)/extra/game_engine -lv4p -lg4p $(CPPFLAGS)