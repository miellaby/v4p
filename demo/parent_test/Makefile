TOP=../..
EXES=parent_test
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/gamemachine

parent_test: parent_test.o libv4p.a $(TOP)/extra/gamemachine/libgm.a
	$(CC) -o $@ parent_test.o -L$(TOP)/extra/gamemachine -lv4p -lgm $(CPPFLAGS)

clean:
	rm -f parent_test *.o