// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef LOG_H
#define LOG_H

void debug(const char *format, ...);
void debug_enable(int enable);
void fatal(const char *format, ...) __attribute__((noreturn));

#endif
