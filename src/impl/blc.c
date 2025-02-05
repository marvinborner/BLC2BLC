// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <log.h>
#include <impl.h>

static void encode(Term *term, FILE *fp)
{
	switch (term->type) {
	case ABS:
		fprintf(fp, "00");
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
		char b = getc(fp);
		if (b == '0') {
			res = term_new(ABS);
			res->u.abs.body = decode(fp);
		} else if (b == '1') {
			res = term_new(APP);
			res->u.app.lhs = decode(fp);
			res->u.app.rhs = decode(fp);
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

Impl impl_blc(void)
{
	return (Impl){
		.name = "blc",
		.encode = encode,
		.decode = decode,
	};
}
