// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef IMPL_H
#define IMPL_H

#include <stdio.h>

#include <term.h>

typedef struct {
	const char *name;
	void (*encode)(Term *, FILE *);
	Term *(*decode)(FILE *);
} Impl;

Impl impl_blc(void);
Impl impl_blc2(void);
Impl impl_closed(void);
Impl impl_app_left(void);
Impl impl_app_right(void);
Impl impl_app_both(void);
Impl impl_abs(void);
Impl impl_abs_app_left(void);

#endif
