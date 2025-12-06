# ----------------------------
# Makefile Options
# ----------------------------

NAME = POKEMON
ICON = icon.png

DESCRIPTION = "Pokemon Battle Factory"

CFLAGS = -Wall -Wextra -Oz -std=c17
CXXFLAGS = -Wall -Wextra -Oz -std=c++20

SRCDIR = src/
BINDIR = bin/
OBJDIR = bin/obj/

COMPRESSED = YES
COMPRESSED_MODE = zx0

ARCHIVED = YES

# ----------------------------

include $(shell cedev-config --makefile)