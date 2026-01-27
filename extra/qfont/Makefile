TOP=../..
LIBS=qfont.o
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine

ifeq ($(MODE),testu)
CPPFLAGS+= -DTESTU_QFONT
testu: qfont
else
testu:
	$(MAKE) MODE=testu clean qfont
endif


qfont: qfont.o $(TOP)/libv4p.a $(TOP)/extra/game_engine/libgm.a 
	$(CC) -o $@ qfont.o \
	  -L$(TOP)/extra/game_engine -lv4p -lgm $(CPPFLAGS)
