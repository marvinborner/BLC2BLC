// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <log.h>
#include <impl.h>

// my blc 3 for now!

static void encode(Term *term, FILE *fp)
{
	switch (term->type) {
	case ABS:
		while (1) {
			fprintf(fp, "0");
			if (term->u.abs.body->type != ABS)
				break;
			term = term->u.abs.body;
		}
		fprintf(fp, "10");
		encode(term->u.abs.body, fp);
		break;
	case APP:;
		Term *stack[128] = { 0 }; // TODO: HEAP!
		int stacked = 0;

		while (1) {
			fprintf(fp, "0");
			stack[stacked++] = term->u.app.rhs;
			term = term->u.app.lhs;
			if (term->type != APP)
				break;
		}

		fprintf(fp, "11");
		encode(term, fp);
		for (int i = stacked - 1; i >= 0; i--)
			encode(stack[i], fp);
		break;
	case IDX:
		for (size_t i = 0; i <= term->u.index; i++)
			fprintf(fp, "1");
		fprintf(fp, "0");
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}

static Term *decode(FILE *fp)
{
	Term *res = 0;

	char a = getc(fp);

	if (a == '0') {
		size_t count = 0;
		while (getc(fp) == '0')
			count++;

		char b = getc(fp);

		if (b == '0') {
			Term *head = term_new(ABS);

			res = head;
			for (size_t i = 0; i < count; i++) {
				res->u.abs.body = term_new(ABS);
				res = res->u.abs.body;
			}
			res->u.abs.body = decode(fp);
			res = head;
		} else if (b == '1') {
			res = term_new(APP);
			res->u.app.lhs = decode(fp);
			res->u.app.rhs = decode(fp);

			for (size_t i = 0; i < count; i++) {
				Term *prev = res;
				res = term_new(APP);
				res->u.app.lhs = prev;
				res->u.app.rhs = decode(fp);
			}
		}
	} else if (a == '1') {
		res = term_new(IDX);
		res->u.index = 0;
		while (getc(fp) == '1')
			res->u.index++;
	}

	if (!res)
		fatal("invalid parsing state!\n");

	return res;
}

Impl impl_abs_app_left(void)
{
	return (Impl){
		.name = "abs_app_left",
		.encode = encode,
		.decode = decode,
	};
}
