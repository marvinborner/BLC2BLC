// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

// only works for closed terms!

// technique proposed by Mateusz Naściszewski (afaik)
// implemented by John Tromp in Haskell

#include <log.h>
#include <impl.h>
#include <stdint.h>

static void encode_index(size_t n, size_t i, FILE *fp)
{
	if (n == 0)
		fatal("invalid index (must be closed)\n");

	if (i == 0) {
		fprintf(fp, "%s", n == 1 ? "1" : "10");
	} else {
		fprintf(fp, "1");
		encode_index(n - 1, i - 1, fp);
	}
}

static void encode_level(size_t n, Term *term, FILE *fp)
{
	switch (term->type) {
	case ABS:
		fprintf(fp, "%s", n == 0 ? "0" : "00");
		encode_level(n + 1, term->u.abs.body, fp);
		break;
	case APP:
		fprintf(fp, "%s", n == 0 ? "1" : "01");
		encode_level(n, term->u.app.lhs, fp);
		encode_level(n, term->u.app.rhs, fp);
		break;
	case IDX:
		encode_index(n, term->u.index, fp);
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}

static void encode(Term *term, FILE *fp)
{
	encode_level(0, term, fp);
}

static Term *decode_level(size_t n, FILE *fp)
{
	Term *res = 0;

	char a = getc(fp);

	if (a == '0' && n == 0) {
		res = term_new(ABS);
		res->u.abs.body = decode_level(n + 1, fp);
	} else if (a == '0') {
		char b = getc(fp);

		if (b == '0') {
			res = term_new(ABS);
			res->u.abs.body = decode_level(n + 1, fp);
		} else if (b == '1') {
			res = term_new(APP);
			res->u.app.lhs = decode_level(n, fp);
			res->u.app.rhs = decode_level(n, fp);
		}
	} else if (a == '1' && n == 0) {
		res = term_new(APP);
		res->u.app.lhs = decode_level(n, fp);
		res->u.app.rhs = decode_level(n, fp);
	} else if (a == '1') {
		res = term_new(IDX);
		res->u.index = 0;
		while (--n != 0 && getc(fp) == '1')
			res->u.index++;
	}

	if (!res)
		fatal("invalid parsing state!\n");

	return res;
}

static Term *decode(FILE *fp)
{
	return decode_level(0, fp);
}

Impl impl_closed(void)
{
	return (Impl){
		.name = "closed",
		.encode = encode,
		.decode = decode,
	};
}
