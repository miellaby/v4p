TOP=../..
EXES=square
include $(TOP)/rules.mak

square: square.o libv4p.a libgm.a
	$(CC) $(CPPFLAGS) -o $@ square.o -L$(TOP) -lv4p -lgm 
