// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <log.h>
#include <impl.h>

static void encode(Term *term, FILE *fp)
{
	switch (term->type) {
	case ABS:
		fprintf(fp, "01");
		encode(term->u.abs.body, fp);
		break;
	case APP:
		fprintf(fp, "0");

		Term *stack[128] = { 0 }; // TODO: HEAP!
		int stacked = 0;

		// TODO: there's probably something here about finding optimal
		//       left/right folds but this is ok for now

		int fold_right = term->u.app.rhs->type == APP;

		while (1) {
			fprintf(fp, "0");
			stack[stacked++] =
				fold_right ? term->u.app.lhs : term->u.app.rhs;
			term = fold_right ? term->u.app.rhs : term->u.app.lhs;
			if (term->type != APP)
				break;
		}

		fprintf(fp, "1%d", fold_right);
		if (fold_right) {
			for (int i = 0; i < stacked; i++)
				encode(stack[i], fp);
			encode(term, fp);
		} else {
			encode(term, fp);
			for (int i = stacked - 1; i >= 0; i--)
				encode(stack[i], fp);
		}
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
			res = term_new(ABS);
			res->u.abs.body = decode(fp);
		} else if (getc(fp) == '1') { // fold right
			Term *head = term_new(APP);
			res = head;
			for (size_t i = 0; i < count - 1; i++) {
				res->u.app.lhs = decode(fp);
				res->u.app.rhs = term_new(APP);
				res = res->u.app.rhs;
			}
			res->u.app.lhs = decode(fp);
			res->u.app.rhs = decode(fp);
			res = head;
		} else { // fold left
			res = term_new(APP);
			res->u.app.lhs = decode(fp);
			res->u.app.rhs = decode(fp);

			for (size_t i = 0; i < count - 1; i++) {
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

Impl impl_app_both(void)
{
	return (Impl){
		.name = "app_both",
		.encode = encode,
		.decode = decode,
	};
}
