# Makefile for Linux Framebuffer V4P backend

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I. -I.. -O2
LDFLAGS = -lm

# Source files
SRCS = v4pi.c
OBJS = $(SRCS:.c=.o)

# Targets
all: $(OBJS)

# Rules
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) *~

# Test target
test: test_fbdev.c $(OBJS)
	$(CC) $(CFLAGS) test_fbdev.c $(OBJS) $(LDFLAGS) -o test_fbdev

.PHONY: all clean test