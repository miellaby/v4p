TOP=../..
EXES=stars
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine
stars: stars.o libv4p.a $(TOP)/extra/gamemachine/libgm.a v4pserial.o
	$(CC) $(CPPFLAGS) -o $@ stars.o ../../v4pserial.o \
	  -L$(TOP)/extra/gamemachine -lv4p -lgm 
