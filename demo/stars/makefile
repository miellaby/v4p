TOP=../..
EXES=stars
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine -I$(TOP)/extra/v4pserial
stars: stars.o libv4p.a $(TOP)/extra/gamemachine/libgm.a $(TOP)/extra/v4pserial/libv4pserial.a
	$(CC) $(CPPFLAGS) -o $@ stars.o \
	  -L$(TOP)/extra/gamemachine -L$(TOP)/extra/v4pserial -lv4p -lv4pserial -lgm 
