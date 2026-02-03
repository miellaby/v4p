# V4P SWIG Bindings Build System
# Autonomous Makefile for generating language bindings

.PHONY: all clean install uninstall lua help

all: lua

# ============================================
# CONFIGURATION
# ============================================

# Tools (can be overridden)
SWIG    ?= swig
CC      ?= gcc
AR      ?= ar
RM      ?= rm -f
MKDIR   ?= mkdir -p

# Build options
TARGET  ?= linux
BACKEND ?= sdl
DEBUG   ?= 0
V       ?= 0
VERBOSE ?= $(V)

# SWIG options
SWIG_OPTS = -Wall -I. -I.. -I../quick -I../backends -I../backends/$(TARGET)

# ============================================
# PLATFORM CONFIGURATION
# ============================================

# Linux platform (default)
ifeq ($(TARGET),linux)
  CC_linux ?= gcc
  AR_linux ?= ar
  CFLAGS_linux =
  CPPFLAGS_linux = -I. -I.. -I../quick -I../backends -I../backends/linux -DV4P_PLATFORM_LINUX
  CC := $(CC_linux)
  AR := $(AR_linux)
endif

# ============================================
# BACKEND CONFIGURATION
# ============================================

# SDL backend (default)
ifeq ($(BACKEND),sdl)
  CPPFLAGS_backend = -I../backends/linux/sdl -DV4P_BACKEND_SDL
  LDFLAGS_backend = -lSDL
  LDLIBS_backend = -lSDL
  CFLAGS_backend =
endif

# Xlib backend
ifeq ($(BACKEND),xlib)
  CPPFLAGS_backend = -I../backends/linux/xlib
  LDFLAGS_backend = -lX11
  LDLIBS_backend = -lX11
  CFLAGS_backend = -DV4P_BACKEND_XLIB
endif

# Framebuffer backend
ifeq ($(BACKEND),fbdev)
  CPPFLAGS_backend = -I../backends/linux/fbdev
  CFLAGS_backend = -DV4P_BACKEND_FBDEV
endif

# DRM backend
ifeq ($(BACKEND),drm)
  CPPFLAGS_backend = -I../backends/linux/drm -I/usr/include/libdrm
  LDFLAGS_backend = -ldrm
  LDLIBS_backend = -ldrm
  CFLAGS_backend = -DV4P_BACKEND_DRM
endif

# ============================================
# COMPILER FLAGS
# ============================================

# Base flags
CFLAGS  = -Wall -Wextra -std=c99 -fPIC
LDFLAGS =
LDLIBS  =

# Add platform and backend specific flags
CFLAGS  += $(CFLAGS_$(TARGET)) $(CFLAGS_backend)
CPPFLAGS += $(CPPFLAGS_$(TARGET)) $(CPPFLAGS_backend)
LDFLAGS += $(LDFLAGS_$(TARGET)) $(LDFLAGS_backend)
LDLIBS  += $(LDLIBS_backend)

# Debug vs Release
ifeq ($(DEBUG),1)
  CFLAGS += -g -O0 -DDEBUG
  CPPFLAGS += -DDEBUG
else
  CFLAGS += -O2 -DNDEBUG
endif

# Verbose output
ifeq ($(VERBOSE),1)
  Q =
  CFLAGS += -Wextra
else
  CFLAGS += -Wno-unused
  Q = @
endif

# ============================================
# LANGUAGE SPECIFIC CONFIGURATION
# ============================================

# Lua configuration
LUA ?= lua
LUA_CFLAGS = $(shell pkg-config --cflags lua 2>/dev/null || echo "")
LUA_LDFLAGS = $(shell pkg-config --libs lua 2>/dev/null || echo "-llua")

# ============================================
# BUILD RULES
# ============================================

# Standard pattern rule
%.o: %.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Pattern rule for files in subdirectories
%/%.o: %/%.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

v4p_wrap.c: v4p.i
	$(Q)$(SWIG) $(SWIG_OPTS) -lua -o $@ $<

lua/v4p.so: v4p_wrap.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(LUA_CFLAGS) -shared -o $@ $< $(LUA_LDFLAGS) -L.. -L../addons/game_engine -lv4p -lg4p

# ============================================
# TARGETS
# ============================================

lua: lua/v4p.so

clean:
	$(Q)$(RM) v4p_wrap.c v4p_wrap.h
	$(Q)$(RM) lua/v4p.so
	$(Q)$(RM) lua/*.o

install:
	$(Q)$(MKDIR) $(DESTDIR)/usr/local/lib/lua/5.4/
	$(Q)cp lua/v4p.so $(DESTDIR)/usr/local/lib/lua/5.4/

uninstall:
	$(Q)$(RM) $(DESTDIR)/usr/local/lib/lua/5.4/v4p.so

help:
	@echo "V4P SWIG Bindings Build System"
	@echo "Usage:"
	@echo "  make                - Build Lua bindings (default)"
	@echo "  make lua            - Build Lua bindings"
	@echo "  make DEBUG=1        - Debug build with symbols"
	@echo "  make BACKEND=xlib   - Use Xlib backend (sdl, xlib, fbdev, drm)"
	@echo "  make TARGET=palmos  - Build for PalmOS (linux, palmos, esp32)"
	@echo "  make V=1            - Verbose output"
	@echo "  make clean          - Clean build artifacts"
	@echo "  make install        - Install Lua bindings"
	@echo "  make help           - Show this help"