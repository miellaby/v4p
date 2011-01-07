TOP=../..
LIBS=qfont.o
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine

ifeq ($(MODE),testu)
testu: qfont
else
testu:
	$(MAKE) MODE=testu clean qfont
endif


qfont: qfont.o $(TOP)/libv4p.a $(TOP)/extra/gamemachine/libgm.a 
	$(CC) $(CPPFLAGS) -o $@ qfont.o \
	  -L$(TOP)/extra/gamemachine -lv4p -lgm 
