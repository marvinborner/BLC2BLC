// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>

#include <term.h>

Term *parse_blc_fp(FILE *fp);
Term *parse_blc(const char **term);

#endif
