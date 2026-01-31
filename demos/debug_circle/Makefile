TOP=../..
EXES=debug_circle
include $(TOP)/rules.mak

# Enable debug mode
MODE=debug

debug_circle: debug_circle.o libv4p.a $(TOP)/addons/game_engine/libg4p.a
	$(CC) -o $@ debug_circle.o -L$(TOP)/addons/game_engine -lv4p -lg4p $(CPPFLAGS)