TOP=../..
LIBS=libv4pserial.a
include $(TOP)/rules.mak

ifeq ($(MODE),testu)
testu: v4pserial_testu
else
testu:
	$(MAKE) MODE=testu clean v4pserial_testu
endif

libv4pserial.a: v4pserial.o
	$(AR) $(ARFLAGS) $@ $?

v4pserial_testu: libv4pserial.a $(TOP)/libv4p.a $(TOP)/extra/gamemachine/libgm.a 
	$(CC) $(CPPFLAGS) -o $@ \
	  -L$(TOP)/extra/gamemachine -lv4p -lgm -lv4pserial 
