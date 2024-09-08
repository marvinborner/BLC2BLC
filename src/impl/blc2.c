// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <log.h>
#include <impl.h>
#include <stdint.h>

// haskell by John Tromp
static void levenshtein_encode(size_t n, FILE *fp)
{
	if (n == 0) {
		fprintf(fp, "10");
		return;
	}

	fprintf(fp, "1");

	size_t length = (8 * sizeof(n)) - __builtin_clzll(n);
	levenshtein_encode(length - 1, fp);

	if (length == 1)
		return;

	for (size_t i = 1 << (length - 2); i != 0; i >>= 1)
		fprintf(fp, "%d", (i & n) != 0);
}

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
		levenshtein_encode(term->u.index, fp);
		break;
	default:
		fatal("invalid type %d\n", term->type);
	}
}

// by John Tromp
static size_t levenshtein_decode(FILE *fp)
{
	if (getc(fp) == '0')
		return 0;
	size_t x, l = levenshtein_decode(fp);
	for (x = 1; l--;)
		x = 2 * x + (getc(fp) != '0');
	return x;
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
		res->u.index = levenshtein_decode(fp);
	}

	if (!res)
		fatal("invalid parsing state!\n");

	return res;
}

Impl impl_blc2(void)
{
	return (Impl){
		.name = "blc2",
		.encode = encode,
		.decode = decode,
	};
}
