// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <parse.h>
#include <log.h>
#include <term.h>

Term *parse_blc_fp(FILE *fp)
{
	Term *res = 0;

	char a = getc(fp);

	if (a == '0') {
		char b = getc(fp);
		if (b == '0') {
			res = term_new(ABS);
			res->u.abs.body = parse_blc_fp(fp);
		} else if (b == '1') {
			res = term_new(APP);
			res->u.app.lhs = parse_blc_fp(fp);
			res->u.app.rhs = parse_blc_fp(fp);
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

Term *parse_blc(const char **term)
{
	Term *res = 0;
	if (!**term) {
		fatal("invalid parsing state!\n");
	} else if (**term == '0' && *(*term + 1) == '0') {
		(*term) += 2;
		res = term_new(ABS);
		res->u.abs.body = parse_blc(term);
	} else if (**term == '0' && *(*term + 1) == '1') {
		(*term) += 2;
		res = term_new(APP);
		res->u.app.lhs = parse_blc(term);
		res->u.app.rhs = parse_blc(term);
	} else if (**term == '1') {
		const char *cur = *term;
		while (**term == '1')
			(*term)++;
		res = term_new(IDX);
		res->u.index = *term - cur - 1;
		(*term)++;
	} else {
		(*term)++;
		res = parse_blc(term);
	}
	return res;
}
