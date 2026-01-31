# Collision test Makefile
TOP=../..
EXES = collision_test
include $(TOP)/rules.mak
CPPFLAGS+= -I$(TOP)/extra/game_engine

collision_test: collision_test.o $(TOP)/libv4p.a
	$(CC) -o $@ collision_test.o \
	  $(TOP)/extra/game_engine/collision.o -L$(TOP) -lv4p $(CPPFLAGS)