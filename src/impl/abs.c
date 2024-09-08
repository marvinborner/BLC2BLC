// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <log.h>
#include <impl.h>

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
		fprintf(fp, "01");
		encode(term->u.abs.body, fp);
		break;
	case APP:
		fprintf(fp, "01");
		encode(term->u.app.lhs, fp);
		encode(term->u.app.rhs, fp);
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

		if (count == 0) {
			res = term_new(APP);
			res->u.app.lhs = decode(fp);
			res->u.app.rhs = decode(fp);
		} else {
			Term *head = term_new(ABS);

			res = head;
			for (size_t i = 0; i < count - 1; i++) {
				res->u.abs.body = term_new(ABS);
				res = res->u.abs.body;
			}
			res->u.abs.body = decode(fp);
			res = head;
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

Impl impl_abs(void)
{
	return (Impl){
		.name = "abs",
		.encode = encode,
		.decode = decode,
	};
}
