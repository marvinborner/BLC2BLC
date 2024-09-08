// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdlib.h>
#include <stdio.h>

#include <term.h>
#include <log.h>
#include <print.h>

Term *term_new(TermType type)
{
	struct term *term = malloc(sizeof(*term));
	if (!term)
		fatal("out of memory!\n");
	term->type = type;
	return term;
}

void term_free(Term *term)
{
	switch (term->type) {
	case ABS:
		term_free(term->u.abs.body);
		free(term);
		break;
	case APP:
		term_free(term->u.app.lhs);
		term_free(term->u.app.rhs);
		free(term);
		break;
	case IDX:
		free(term);
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}

void term_diff(Term *a, Term *b)
{
	if (a->type != b->type) {
		fprintf(stderr, "Term a: ");
		print_bruijn(a);
		fprintf(stderr, "\nTerm b: ");
		print_bruijn(b);
		fprintf(stderr, "\n");
		fatal("type mismatch %d %d\n", a->type, b->type);
	}

	switch (a->type) {
	case ABS:
		term_diff(a->u.abs.body, b->u.abs.body);
		break;
	case APP:
		term_diff(a->u.app.lhs, b->u.app.lhs);
		term_diff(a->u.app.rhs, b->u.app.rhs);
		break;
	case IDX:
		if (a->u.index != b->u.index)
			fatal("var mismatch %d=%d\n", a->u.index, b->u.index);
		break;
	default:
		fatal("invalid type %d\n", a->type);
	}
}
