// Copyright (c) 2024, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#include <term.h>
#include <parse.h>
#include <log.h>
#include <print.h>
#include <impl.h>

#define N_IMPLS 8

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
	double results[N_IMPLS] = { 0 };

	char path[1024];
	struct dirent *dir_entry;
	DIR *dir = opendir(dir_path);
	while ((dir_entry = readdir(dir))) {
		if (dir_entry->d_type != DT_REG)
			continue;

		sprintf(path, "%s/%s", dir_path, dir_entry->d_name);
		debug("%s\n", path);

		FILE *fp = fopen(path, "rb");

		Term *term = parse_blc_fp(fp);

		for (Impl *impl = impls; impl != impls + N_IMPLS; impl++) {
			size_t prev = file_size(fp);
			size_t after = test_impl(*impl, term, out);
			double change = (double)((long)prev - (long)after) /
					(double)prev * 100;
			debug("%s: %lu -> %lu (%f%% reduction)\n", impl->name,
			      prev, after, change);
			results[impl - impls] += change;
			results[impl - impls] /= 2.0;
		}

		term_free(term);
		fclose(fp);

		debug("\n");
	}
	closedir(dir);

	printf("\n--- %s ---\n", dir_path);
	for (size_t i = 0; i < N_IMPLS; i++)
		printf("%s: avg. %f%% reduction\n", impls[i].name, results[i]);
}

static void test(Impl impls[N_IMPLS])
{
	FILE *out = tmpfile();

	const char *dir_path = "test/small";
	test_all(impls, dir_path, out);
	dir_path = "test/medium";
	test_all(impls, dir_path, out);
	dir_path = "test/large";
	test_all(impls, dir_path, out);

	fclose(out);
}

static Impl find_impl(const char *impl, Impl impls[N_IMPLS])
{
	for (int i = 0; i < N_IMPLS; i++) {
		if (!strcmp(impl, impls[i].name))
			return impls[i];
	}
	fatal("encoding '%s' not found!\n", impl);
}

int main(int argc, char *argv[])
{
#ifdef DEBUG
	debug_enable(DEBUG);
#endif
	Impl impls[N_IMPLS] = { impl_blc(),	 impl_blc2(),
				impl_closed(),	 impl_abs(),
				impl_app_left(), impl_app_right(),
				impl_app_both(), impl_abs_app_left() };
	if (argc == 1) {
		test(impls);
		return 0;
	}

	if (argc != 3) {
		fatal("invalid arguments, please use `blc2blc <from> <to>`\n");
	}

	Impl from = find_impl(argv[1], impls);
	Impl to = find_impl(argv[2], impls);
	Term *term = from.decode(stdin);
	to.encode(term, stdout);

	return 0;
}
