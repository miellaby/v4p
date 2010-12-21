TOP=../..
EXES=stars
include $(TOP)/rules.mak

stars: stars.o libv4p.a libgm.a v4pserial.o
	$(CC) $(CPPFLAGS) -o $@ stars.o ../../v4pserial.o -L$(TOP) -lv4p -lgm 
