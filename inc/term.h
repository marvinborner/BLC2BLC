// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef TERM_H
#define TERM_H

#include <stddef.h>

typedef enum { INV, ABS, APP, IDX } TermType;

typedef struct term {
	TermType type;
	union {
		struct {
			struct term *body;
		} abs;
		struct {
			struct term *lhs;
			struct term *rhs;
		} app;
		size_t index;
	} u;
} Term;

struct term *term_new(TermType type);
void term_free(Term *term);
void term_diff(Term *a, Term *b);

#endif
