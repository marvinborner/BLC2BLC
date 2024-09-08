// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include <term.h>
#include <parse.h>
#include <print.h>
#include <impl.h>

#define N_IMPLS 5

static size_t file_size(FILE *fp)
{
	struct stat st;
	fstat(fileno(fp), &st);
	return st.st_size;
}

static size_t test_impl(Impl impl, Term *term, FILE *out)
{
	rewind(out);
	impl.encode(term, out);
	size_t size = ftell(out);
	rewind(out);

	Term *recon = impl.decode(out);
	term_diff(term, recon);
	term_free(recon);

	return size;
}

static void test_all(Impl impls[N_IMPLS], const char *dir_path, FILE *out)
{
	char path[1024];
	struct dirent *dir_entry;
	DIR *dir = opendir(dir_path);
	while ((dir_entry = readdir(dir))) {
		if (dir_entry->d_type != DT_REG)
			continue;

		sprintf(path, "%s/%s", dir_path, dir_entry->d_name);
		printf("%s\n", path);

		FILE *fp = fopen(path, "rb");

		Term *term = parse_blc_fp(fp);

		for (Impl *impl = impls; impl != impls + N_IMPLS; impl++) {
			size_t prev = file_size(fp);
			size_t after = test_impl(*impl, term, out);
			printf("%s: %lu -> %lu (%f%% reduction)\n", impl->name,
			       prev, after,
			       (double)((long)prev - (long)after) /
				       (double)prev * 100);
		}

		term_free(term);
		fclose(fp);

		printf("\n");
	}
	closedir(dir);
}

int main(void)
{
	Impl impls[N_IMPLS] = { impl_blc(), impl_blc2(), impl_closed(),
				impl_abs(), impl_app() };

	FILE *out = tmpfile();

	const char *dir_path = "test/small";
	test_all(impls, dir_path, out);
	dir_path = "test/medium";
	test_all(impls, dir_path, out);
	/* dir_path = "test/large"; */
	/* test_all(impls, dir_path, out); */

	fclose(out);
}
