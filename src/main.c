// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdio.h>

#include <term.h>
#include <parse.h>
#include <print.h>

int main(void)
{
	Term *term = parse_blc_fp(stdin);
	print_bruijn(term);
	printf("\n");
}
