# Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
# SPDX-License-Identifier: MIT

CC = gcc
TG = ctags

BUILD = ${CURDIR}/build
SRC = ${CURDIR}/src
INC = ${CURDIR}/inc
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(SRCS))

CFLAGS_DEBUG = -fsanitize=address,leak,undefined -g -O0
CFLAGS_WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wredundant-decls -Wnested-externs -Wmissing-declarations -Wstrict-prototypes -Wmissing-prototypes -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code -Wundef -Wold-style-definition -pedantic -Wno-switch-enum
CFLAGS = $(CFLAGS_WARNINGS) -std=c99 -Ofast -I$(INC)

ifdef DEBUG # TODO: Somehow clean automagically
CFLAGS += $(CFLAGS_DEBUG)
endif

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

all: compile

full: all sync

compile: $(BUILD) $(OBJS) $(BUILD)/blc

clean:
	@rm -rf $(BUILD)/*

install:
	@install -m 755 $(BUILD)/blc $(DESTDIR)$(PREFIX)/bin/

sync: # Ugly hack
	@$(MAKE) $(BUILD)/blc --always-make --dry-run | grep -wE 'gcc|g\+\+' | grep -w '\-c' | jq -nR '[inputs|{directory:".", command:., file: match(" [^ ]+$$").string[1:]}]' >compile_commands.json
	@$(TG) -R --exclude=.git --exclude=build .

$(BUILD)/%.o: $(SRC)/%.c
	@$(CC) -c -o $@ $(CFLAGS) $<

$(BUILD)/blc: $(OBJS)
	@$(CC) -o $@ $(CFLAGS) $^

.PHONY: all compile clean sync

$(BUILD):
	@mkdir -p $@
