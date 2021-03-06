#include "clar_libgit2.h"
#include "git2/sys/repository.h"

#include "apply.h"
#include "repository.h"

#include "../patch/patch_common.h"

static git_repository *repo = NULL;
static git_diff_options binary_opts = GIT_DIFF_OPTIONS_INIT;

void test_apply_fromdiff__initialize(void)
{
	repo = cl_git_sandbox_init("renames");

	binary_opts.flags |= GIT_DIFF_SHOW_BINARY;
}

void test_apply_fromdiff__cleanup(void)
{
	cl_git_sandbox_cleanup();
}

static int apply_gitbuf(
	const git_str *old,
	const char *oldname,
	const git_str *new,
	const char *newname,
	const char *patch_expected,
	const git_diff_options *diff_opts)
{
	git_patch *patch;
	git_str result = GIT_STR_INIT;
	git_buf patchbuf = GIT_BUF_INIT;
	char *filename;
	unsigned int mode;
	int error;

	cl_git_pass(git_patch_from_buffers(&patch,
		old ? old->ptr : NULL, old ? old->size : 0,
		oldname,
		new ? new->ptr : NULL, new ? new->size : 0,
		newname,
		diff_opts));

	if (patch_expected) {
		cl_git_pass(git_patch_to_buf(&patchbuf, patch));
		cl_assert_equal_s(patch_expected, patchbuf.ptr);
	}

	error = git_apply__patch(&result, &filename, &mode, old ? old->ptr : NULL, old ? old->size : 0, patch, NULL);

	if (error == 0 && new == NULL) {
		cl_assert_equal_i(0, result.size);
		cl_assert_equal_p(NULL, filename);
		cl_assert_equal_i(0, mode);
	}
	else if (error == 0) {
		cl_assert_equal_s(new->ptr, result.ptr);
		cl_assert_equal_s(newname ? newname : oldname, filename);
		cl_assert_equal_i(0100644, mode);
	}

	git__free(filename);
	git_str_dispose(&result);
	git_buf_dispose(&patchbuf);
	git_patch_free(patch);

	return error;
}

static int apply_buf(
	const char *old,
	const char *oldname,
	const char *new,
	const char *newname,
	const char *patch_expected,
	const git_diff_options *diff_opts)
{
	git_str o = GIT_STR_INIT, n = GIT_STR_INIT,
		*optr = NULL, *nptr = NULL;

	if (old) {
		o.ptr = (char *)old;
		o.size = strlen(old);
		optr = &o;
	}

	if (new) {
		n.ptr = (char *)new;
		n.size = strlen(new);
		nptr = &n;
	}

	return apply_gitbuf(optr, oldname, nptr, newname, patch_expected, diff_opts);
}

void test_apply_fromdiff__change_middle(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_CHANGE_MIDDLE, "file.txt",
		PATCH_ORIGINAL_TO_CHANGE_MIDDLE, NULL));
}

void test_apply_fromdiff__change_middle_nocontext(void)
{
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	diff_opts.context_lines = 0;

	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_CHANGE_MIDDLE, "file.txt",
		PATCH_ORIGINAL_TO_CHANGE_MIDDLE_NOCONTEXT, &diff_opts));
}

void test_apply_fromdiff__change_firstline(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_CHANGE_FIRSTLINE, "file.txt",
		PATCH_ORIGINAL_TO_CHANGE_FIRSTLINE, NULL));
}

void test_apply_fromdiff__lastline(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_CHANGE_LASTLINE, "file.txt",
		PATCH_ORIGINAL_TO_CHANGE_LASTLINE, NULL));
}

void test_apply_fromdiff__change_middle_and_lastline_nocontext(void)
{
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	diff_opts.context_lines = 0;

	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_CHANGE_MIDDLE_AND_LASTLINE, "file.txt",
		PATCH_ORIGINAL_TO_CHANGE_MIDDLE_AND_LASTLINE_NOCONTEXT, &diff_opts));
}

void test_apply_fromdiff__prepend(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_PREPEND, "file.txt",
		PATCH_ORIGINAL_TO_PREPEND, NULL));
}

void test_apply_fromdiff__prepend_nocontext(void)
{
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	diff_opts.context_lines = 0;

	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_PREPEND, "file.txt",
		PATCH_ORIGINAL_TO_PREPEND_NOCONTEXT, &diff_opts));
}

void test_apply_fromdiff__prepend_and_change(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_PREPEND_AND_CHANGE, "file.txt",
		PATCH_ORIGINAL_TO_PREPEND_AND_CHANGE, NULL));
}

void test_apply_fromdiff__prepend_and_change_nocontext(void)
{
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	diff_opts.context_lines = 0;

	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_PREPEND_AND_CHANGE, "file.txt",
		PATCH_ORIGINAL_TO_PREPEND_AND_CHANGE_NOCONTEXT, &diff_opts));
}

void test_apply_fromdiff__delete_and_change(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_DELETE_AND_CHANGE, "file.txt",
		PATCH_ORIGINAL_TO_DELETE_AND_CHANGE, NULL));
}

void test_apply_fromdiff__delete_and_change_nocontext(void)
{
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	diff_opts.context_lines = 0;

	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_DELETE_AND_CHANGE, "file.txt",
		PATCH_ORIGINAL_TO_DELETE_AND_CHANGE_NOCONTEXT, &diff_opts));
}

void test_apply_fromdiff__delete_firstline(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_DELETE_FIRSTLINE, "file.txt",
		PATCH_ORIGINAL_TO_DELETE_FIRSTLINE, NULL));
}

void test_apply_fromdiff__append(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_APPEND, "file.txt",
		PATCH_ORIGINAL_TO_APPEND, NULL));
}

void test_apply_fromdiff__append_nocontext(void)
{
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	diff_opts.context_lines = 0;

	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_APPEND, "file.txt",
		PATCH_ORIGINAL_TO_APPEND_NOCONTEXT, &diff_opts));
}

void test_apply_fromdiff__prepend_and_append(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_PREPEND_AND_APPEND, "file.txt",
		PATCH_ORIGINAL_TO_PREPEND_AND_APPEND, NULL));
}

void test_apply_fromdiff__to_empty_file(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		"", NULL,
		PATCH_ORIGINAL_TO_EMPTY_FILE, NULL));
}

void test_apply_fromdiff__from_empty_file(void)
{
	cl_git_pass(apply_buf(
		"", NULL,
		FILE_ORIGINAL, "file.txt",
		PATCH_EMPTY_FILE_TO_ORIGINAL, NULL));
}

void test_apply_fromdiff__add(void)
{
	cl_git_pass(apply_buf(
		NULL, NULL,
		FILE_ORIGINAL, "file.txt",
		PATCH_ADD_ORIGINAL, NULL));
}

void test_apply_fromdiff__delete(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		NULL, NULL,
		PATCH_DELETE_ORIGINAL, NULL));
}

void test_apply_fromdiff__no_change(void)
{
	cl_git_pass(apply_buf(
		FILE_ORIGINAL, "file.txt",
		FILE_ORIGINAL, "file.txt",
		"", NULL));
}

void test_apply_fromdiff__binary_add(void)
{
	git_str newfile = GIT_STR_INIT;

	newfile.ptr = FILE_BINARY_DELTA_MODIFIED;
	newfile.size = FILE_BINARY_DELTA_MODIFIED_LEN;

	cl_git_pass(apply_gitbuf(
		NULL, NULL,
		&newfile, "binary.bin",
		NULL, &binary_opts));
}

void test_apply_fromdiff__binary_no_change(void)
{
	git_str original = GIT_STR_INIT;

	original.ptr = FILE_BINARY_DELTA_ORIGINAL;
	original.size = FILE_BINARY_DELTA_ORIGINAL_LEN;

	cl_git_pass(apply_gitbuf(
		&original, "binary.bin",
		&original, "binary.bin",
		"", &binary_opts));
}

void test_apply_fromdiff__binary_change_delta(void)
{
	git_str original = GIT_STR_INIT, modified = GIT_STR_INIT;

	original.ptr = FILE_BINARY_DELTA_ORIGINAL;
	original.size = FILE_BINARY_DELTA_ORIGINAL_LEN;

	modified.ptr = FILE_BINARY_DELTA_MODIFIED;
	modified.size = FILE_BINARY_DELTA_MODIFIED_LEN;

	cl_git_pass(apply_gitbuf(
		&original, "binary.bin",
		&modified, "binary.bin",
		NULL, &binary_opts));
}

void test_apply_fromdiff__binary_change_literal(void)
{
	git_str original = GIT_STR_INIT, modified = GIT_STR_INIT;

	original.ptr = FILE_BINARY_LITERAL_ORIGINAL;
	original.size = FILE_BINARY_LITERAL_ORIGINAL_LEN;

	modified.ptr = FILE_BINARY_LITERAL_MODIFIED;
	modified.size = FILE_BINARY_LITERAL_MODIFIED_LEN;

	cl_git_pass(apply_gitbuf(
		&original, "binary.bin",
		&modified, "binary.bin",
		NULL, &binary_opts));
}

void test_apply_fromdiff__binary_delete(void)
{
	git_str original = GIT_STR_INIT;

	original.ptr = FILE_BINARY_DELTA_MODIFIED;
	original.size = FILE_BINARY_DELTA_MODIFIED_LEN;

	cl_git_pass(apply_gitbuf(
		&original, "binary.bin",
		NULL, NULL,
		NULL, &binary_opts));
}

void test_apply_fromdiff__patching_correctly_truncates_source(void)
{
	git_str original = GIT_STR_INIT, patched = GIT_STR_INIT;
	git_patch *patch;
	unsigned int mode;
	char *path;

	cl_git_pass(git_patch_from_buffers(&patch,
					   "foo\nbar", 7, "file.txt",
					   "foo\nfoo", 7, "file.txt", NULL));

	/*
	 * Previously, we would fail to correctly truncate the source buffer if
	 * the source has more than one line and ends with a non-newline
	 * character. In the following call, we thus truncate the source string
	 * in the middle of the second line. Without the bug fixed, we would
	 * successfully apply the patch to the source and return success. With
	 * the overflow being fixed, we should return an error.
	 */
	cl_git_fail_with(GIT_EAPPLYFAIL,
			 git_apply__patch(&patched, &path, &mode,
					  "foo\nbar\n", 5, patch, NULL));

	/* Verify that the patch succeeds if we do not truncate */
	cl_git_pass(git_apply__patch(&patched, &path, &mode,
				     "foo\nbar\n", 7, patch, NULL));

	git_str_dispose(&original);
	git_str_dispose(&patched);
	git_patch_free(patch);
	git__free(path);
}
