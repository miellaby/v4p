# V4P Build System - Single Makefile
# Modern, standards-compliant build system

.PHONY: all clean install uninstall addons demos help
.SECONDARY: # Prevents intermediate files from being deleted (I hate that)

all: libv4p.a addons demos
ifneq ($(TARGET),emscripten)
  all: tests
endif

# ============================================
# CONFIGURATION
# ============================================

# Tools (can be overridden)
CC      ?= gcc
AR      ?= ar
RM      ?= rm -f
MKDIR   ?= mkdir -p

# Build options with modern interface
TARGET  ?= linux
BACKEND ?= sdl
DEBUG   ?= 0
V       ?= 0
VERBOSE ?= $(V)
PREFIX  ?= /usr/local

# ============================================
# PLATFORM CONFIGURATION
# ============================================

# Linux platform (default)
ifeq ($(TARGET),linux)
  CC_linux ?= gcc
  AR_linux ?= ar
  CFLAGS_linux =
  CPPFLAGS_linux = -I. -Iquick -Ibackends -Ibackends/linux -DV4P_PLATFORM_LINUX
  CC := $(CC_linux)
  AR := $(AR_linux)
endif

# PalmOS platform
ifeq ($(TARGET),palmos)
  CC_palmos ?= m68k-palmos-gcc
  AR_palmos ?= m68k-palmos-ar
  CFLAGS_palmos = -fno_builtin
  CPPFLAGS_palmos = -I. -Iquick -Ibackends -Ibackends/palmos -I$(PALMOS_INCLUDE) -DV4P_PLATFORM_PALMOS
  CC := $(CC_palmos)
  AR := $(AR_palmos)
endif

# ESP32 platform (future)
ifeq ($(TARGET),esp32)
  CC_esp32 ?= xtensa-esp32-elf-gcc
  AR_esp32 ?= xtensa-esp32-elf-ar
  CFLAGS_esp32 = -mfix-esp32-psram-cache-issue
  CPPFLAGS_esp32 = -I. -Iquick -Ibackends -Ibackends/esp32 -DV4P_PLATFORM_ESP32
  LDFLAGS_esp32 = -Wl,--gc-sections -Wl,-static
  CC := $(CC_esp32)
  AR := $(AR_esp32)
endif

# Emscripten platform
ifeq ($(TARGET),emscripten)
  CC_emscripten ?= emcc
  AR_emscripten ?= emar
  CFLAGS_emscripten =
  CPPFLAGS_emscripten = -I. -Iquick -Ibackends -Ibackends/emscripten -DV4P_PLATFORM_EMSCRIPTEN
  LDFLAGS_emscripten = -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s ASYNCIFY -s EXPORTED_FUNCTIONS='["_g4p_setFramerate", "_g4p_getFramerate", "_main"]'
  CC := $(CC_emscripten)
  AR := $(AR_emscripten)
endif

# ============================================
# BACKEND CONFIGURATION
# ============================================

# SDL backend (default)
ifeq ($(BACKEND),sdl)
  CPPFLAGS_backend = -Ibackends/linux/sdl -DV4P_BACKEND_SDL
  LDFLAGS_backend = -lSDL
  LDLIBS_backend = -lSDL
  CFLAGS_backend =
endif

# Xlib backend
ifeq ($(BACKEND),xlib)
  CPPFLAGS_backend = -Ibackends/linux/xlib
  LDFLAGS_backend = -lX11
  LDLIBS_backend = -lX11
  CFLAGS_backend = -DV4P_BACKEND_XLIB
endif

# Framebuffer backend
ifeq ($(BACKEND),fbdev)
  CPPFLAGS_backend = -Ibackends/linux/fbdev
  CFLAGS_backend = -DV4P_BACKEND_FBDEV
endif

# DRM backend
ifeq ($(BACKEND),drm)
  CPPFLAGS_backend = -Ibackends/linux/drm -I/usr/include/libdrm
  LDFLAGS_backend = -ldrm
  LDLIBS_backend = -ldrm
  CFLAGS_backend = -DV4P_BACKEND_DRM
endif

# libcaca backend
ifeq ($(BACKEND),caca)
  CPPFLAGS_backend = -Ibackends/linux/caca
  LDFLAGS_backend = -lcaca
  LDLIBS_backend = -lcaca
  CFLAGS_backend = -DV4P_BACKEND_CACA
endif

# Canvas backend (for emscripten target)
ifeq ($(BACKEND),canvas)
  CPPFLAGS_backend = -Ibackends/canvas
  CFLAGS_backend = -DV4P_BACKEND_CANVAS
  LDFLAGS_backend = 
  LDLIBS_backend = 
endif

# DOM backend (for emscripten target)
ifeq ($(BACKEND),dom)
  CPPFLAGS_backend = -Ibackends/dom
  CFLAGS_backend = -DV4P_BACKEND_DOM
  LDFLAGS_backend = 
  LDLIBS_backend = 
endif

# Bitmap backend (for emscripten target)
ifeq ($(BACKEND),bitmap)
  CPPFLAGS_backend = -Ibackends/bitmap
  CFLAGS_backend = -DV4P_BACKEND_BITMAP
  LDFLAGS_backend = 
  LDLIBS_backend = 
endif

# ============================================
# COMPILER FLAGS
# ============================================

# Base flags
CPPFLAGS = -Iaddons/game_engine -Iaddons/v4pserial -Iaddons/qfont -Iaddons/particles -I/usr/include/lua5.1
CFLAGS  = -Wall -Wextra -std=gnu99 -fPIC
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
  ifeq ($(ASAN),1)
    CFLAGS += -fsanitize=address -fno-omit-frame-pointer
    LDFLAGS += -fsanitize=address -static-libasan
  endif
else
  CFLAGS += -O2 -DNDEBUG
endif

# Trace system configuration
ifdef TRACE
  # 1. Define the master list of available tags
  TRACE_TAGS := CIRCLE POLYGON SCAN EDGE SHIFT OPEN COLLISION RENDER TRANSFORM G4P

  ifeq ($(TRACE),all)
    TRACE := $(TRACE_TAGS)
  endif

  # 2. For each tag, if it's found in $(TRACE), add -DTRACE_TAG=1, else -DTRACE_TAG=0
  CPPFLAGS += $(foreach tag,$(TRACE_TAGS), \
    $(if $(findstring $(tag),$(TRACE)),-DTRACE_$(tag)=1,-DTRACE_$(tag)=0))
    
  CPPFLAGS += -DTRACE
  $(info Enabling trace tags: $(filter $(TRACE_TAGS),$(TRACE)))
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
# SOURCE FILES
# ============================================

# Core library
CORE_SRCS = \
    v4p.c v4p_color.c \
    quick/quickheap.c quick/quicktable.c quick/sortable.c quick/lowmath.c \
    backends/$(TARGET)/v4p_ll.c

BACKEND_SRCS = backends/$(TARGET)/$(BACKEND)/v4pi.c

# Addons
GAME_ENGINE_SRCS = addons/game_engine/g4p.c addons/game_engine/$(TARGET)/$(BACKEND)/g4pi.c addons/game_engine/collision.c
QFONT_SRCS = addons/qfont/qfont.c
V4PSERIAL_SRCS = addons/v4pserial/v4pserial.c
PARTICLES_SRCS = addons/particles/particles.c
DEBUG_SRCS = addons/debug/debug.c

# ============================================
# BUILD RULES
# ============================================

# Standard pattern rule
%.o: %.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%/%.o: %/%.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Demos - build object files in demo directories
demos/%.o: demos/%.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -Iaddons/game_engine -Iaddons/v4pserial -Iaddons/qfont -Iaddons/debug -c $< -o $@

# Link demos in their directories
demos/%: demos/%.o libv4p.a libg4p.a libqfont.a libv4pserial.a libparticles.a libdebug.a
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lm

# Emscripten demo pages
TEMPLATE_FILE := web/templates/$(BACKEND)-template.html
demos/%.html: demos/%.o $(TEMPLATE_FILE) libv4p.a libg4p.a libqfont.a libv4pserial.a libparticles.a libdebug.a
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o demos/$*.js $< libv4p.a libg4p.a libqfont.a libv4pserial.a libparticles.a libdebug.a $(LDLIBS) -lm
	$(Q)sed "s|{{DEMO_NAME}}|$*|g; s|{{DEMO_TITLE}}|$* Demo|g" $(TEMPLATE_FILE) > $@
	$(Q)echo "Generated: $@ and demos/$*.js"

# Tests - build object files in demo directories
tests/%.o: tests/%.c # Note: It is recommanded to build tests with DEBUG=1 for better diagnostics
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -DDEBUG=1 -Iaddons/game_engine -Iaddons/v4pserial -Iaddons/qfont -c $< -o $@

# Link tests in their directories
tests/%: tests/%.o libv4p.a libg4p.a libqfont.a libv4pserial.a libparticles.a
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lm

# ============================================
# TARGETS
# ============================================

# Core library
libv4p.a: $(patsubst %.c,%.o,$(CORE_SRCS)) $(patsubst %.c,%.o,$(BACKEND_SRCS))
	$(Q)$(AR) rcs $@ $^

# Addons
addons: libg4p.a libqfont.a libv4pserial.a libparticles.a libdebug.a

libg4p.a: $(patsubst %.c,%.o,$(GAME_ENGINE_SRCS))
	$(Q)$(AR) rcs $@ $^

libqfont.a: $(patsubst %.c,%.o,$(QFONT_SRCS))
	$(Q)$(AR) rcs $@ $^

libv4pserial.a: $(patsubst %.c,%.o,$(V4PSERIAL_SRCS))
	$(Q)$(AR) rcs $@ $^

libparticles.a: $(patsubst %.c,%.o,$(PARTICLES_SRCS))
	$(Q)$(AR) rcs $@ $^

libdebug.a: $(patsubst %.c,%.o,$(DEBUG_SRCS))
	$(Q)$(AR) rcs $@ $^

# Demos target - build all available demos
ifeq ($(TARGET),emscripten)
DEMO_TARGETS := $(patsubst demos/%.c,demos/%.html,$(wildcard demos/*.c))
else
DEMO_TARGETS := $(patsubst demos/%.c,demos/%,$(wildcard demos/*.c))
endif
demos: $(DEMO_TARGETS)

TEST_TARGETS := $(patsubst tests/%.c,tests/%,$(wildcard tests/*.c))
tests: $(TEST_TARGETS)

clean:
	$(Q)$(RM) *.o *.a
	$(Q)$(RM) quick/*.o
	$(Q)$(RM) backends/*.o backends/*/*.o
	$(Q)$(RM) addons/*/*.o
	$(Q)$(RM) addons/*/*.a
	$(Q)$(RM) demos/*.o
	$(Q)$(RM) $(patsubst demos/%.c,demos/%,$(wildcard demos/*.c))
	$(Q)$(RM) $(patsubst tests/%.c,tests/%,$(wildcard tests/*.c))
	$(Q)$(RM) -rf demos/web
	$(Q)$(RM) demos/*.html demos/*.js demos/*.wasm

install: libv4p.a
	$(Q)$(MKDIR) $(PREFIX)/lib
	$(Q)$(MKDIR) $(PREFIX)/include
	$(Q)cp libv4p.a $(PREFIX)/lib/
	$(Q)cp v4p.h $(PREFIX)/include/

uninstall:
	$(Q)$(RM) $(PREFIX)/lib/libv4p.a
	$(Q)$(RM) $(PREFIX)/include/v4p.h

help:
	@echo "V4P Build System - Modern Interface"
	@echo "Usage:"
	@echo "  make                - Release build (default)"
	@echo "  make DEBUG=1        - Debug build with symbols"
	@echo "  make DEBUG=1 ASAN=1 - Debug build with AddressSanitizer"
	@echo "  make TARGET=emscripten - Build for WASM (linux, emscripten, palmos, esp32)"
	@echo "  make BACKEND=xlib   - Use Xlib backend (linux: sdl, xlib, fbdev, drm, caca) (emscripten: canvas, dom, bitmap)"
	@echo "  make V=1            - Verbose output"
	@echo "  make PREFIX=/opt    - Custom install prefix"
	@echo "  make install        - Install to system"
	@echo "  make clean          - Clean build artifacts"
	@echo "  make help           - Show this help"
