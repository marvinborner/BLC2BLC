// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdio.h>

#include <print.h>
#include <log.h>

void print_bruijn(Term *term)
{
	switch (term->type) {
	case ABS:
		fprintf(stderr, "[");
		print_bruijn(term->u.abs.body);
		fprintf(stderr, "]");
		break;
	case APP:
		fprintf(stderr, "(");
		print_bruijn(term->u.app.lhs);
		fprintf(stderr, " ");
		print_bruijn(term->u.app.rhs);
		fprintf(stderr, ")");
		break;
	case IDX:
		fprintf(stderr, "%lu", term->u.index);
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}

void print_blc(Term *term)
{
	switch (term->type) {
	case ABS:
		printf("00");
		print_blc(term->u.abs.body);
		break;
	case APP:
		printf("01");
		print_blc(term->u.app.lhs);
		print_blc(term->u.app.rhs);
		break;
	case IDX:
		for (size_t i = 0; i <= term->u.index; i++)
			printf("1");
		printf("0");
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}
