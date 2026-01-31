TOP=../..
EXES=qfont_demo
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/addons/game_engine -I$(TOP)/addons/qfont

qfont_demo: qfont_demo.o libv4p.a $(TOP)/addons/game_engine/libg4p.a $(TOP)/addons/qfont/qfont.o
	$(CC) -o $@ qfont_demo.o $(TOP)/addons/qfont/qfont.o \
	  -L$(TOP)/addons/game_engine -lv4p -lg4p $(CPPFLAGS)