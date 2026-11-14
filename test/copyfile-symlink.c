#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "check.h"

static void check_link(const char *path, const char *target)
{
	char buf[256];
	struct stat st;
	ssize_t len;

	fail_unless(lstat(path, &st) == 0 && S_ISLNK(st.st_mode));
	len = readlink(path, buf, sizeof(buf));
	fail_unless(len == (ssize_t)strlen(target));
	fail_unless(memcmp(buf, target, len) == 0);
}

/* A link left in the destination by an earlier run must never be written through. */
static void rsync_stale_links(void)
{
	struct stat st;
	FILE *fp;

	fail_unless(mkdir("src", 0700) == 0 && mkdir("dst", 0700) == 0);
	fail_unless(touch("victim") == 0);
	fail_unless(symlink("../victim", "src/a") == 0);
	fail_unless(symlink("../victims", "src/d") == 0);
	fail_unless(rsync("src/", "dst/", LITE_FOPT_RSYNC_LINKS, NULL) == 0);
	check_link("dst/a", "../victim");
	check_link("dst/d", "../victims");

	/* The source links become a file and a directory, and dst/d stops dangling. */
	fail_unless(unlink("src/a") == 0 && unlink("src/d") == 0);
	fp = fopen("src/a", "w");
	fail_unless(fp && fputs("data", fp) >= 0 && fclose(fp) == 0);
	fail_unless(mkdir("src/d", 0700) == 0 && touch("src/d/f") == 0);
	fail_unless(mkdir("victims", 0700) == 0);
	fail_unless(rsync("src/", "dst/", LITE_FOPT_RSYNC_LINKS, NULL) == 0);

	fail_unless(lstat("dst/a", &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 4);
	fail_unless(stat("victim", &st) == 0 && st.st_size == 0);
	fail_unless(lstat("dst/d", &st) == 0 && S_ISDIR(st.st_mode));
	fail_unless(fexist("dst/d/f") && !fexist("victims/f"));

	fail_unless(unlink("src/a") == 0 && unlink("src/d/f") == 0 && rmdir("src/d") == 0);
	fail_unless(unlink("dst/a") == 0 && unlink("dst/d/f") == 0 && rmdir("dst/d") == 0);
	fail_unless(rmdir("src") == 0 && rmdir("dst") == 0);
	fail_unless(unlink("victim") == 0 && rmdir("victims") == 0);
}

int main(void)
{
	char tmp[] = "/tmp/libite-copylink-XXXXXX";
	const char payload[] = "target contents";
	char buf[sizeof(payload)];
	const char *targets[] = { "file", "dir", "missing", ".", "link" };
	struct timespec times[2] = { { 1234567800, 0 }, { 1234567801, 0 } };
	struct stat src_st, dst_st;
	char cwd[4096];
	size_t i;
	FILE *fp;

	fail_unless(getcwd(cwd, sizeof(cwd)) && mkdtemp(tmp));
	fail_unless(chdir(tmp) == 0);
	fail_unless(mkdir("dir", 0700) == 0);
	fail_unless(mkdir("out", 0700) == 0);
	fp = fopen("file", "w");
	fail_unless(fp && fwrite(payload, 1, sizeof(payload), fp) == sizeof(payload));
	fail_unless(fclose(fp) == 0);

	for (i = 0; i < NELEMS(targets); i++) {
		fail_unless(symlink(targets[i], "link") == 0);
		fail_unless(copyfile("link", "copied", 0, LITE_FOPT_COPYFILE_SYM) == 1);
		check_link("copied", targets[i]);
		fail_unless(copyfile("link", "out", 0, LITE_FOPT_COPYFILE_SYM) == 1);
		check_link("out/link", targets[i]);
		/* Existing targets must survive a failed symlink creation. */
		errno = 0;
		fail_unless(copyfile("link", "file", 0, LITE_FOPT_COPYFILE_SYM) == 0);
		fail_unless(errno == EEXIST);
		check_link("copied", targets[i]);
		fail_unless(unlink("copied") == 0 && unlink("out/link") == 0);
		fail_unless(unlink("link") == 0);
	}

	fail_unless(symlink("file", "link") == 0);
	/* Without SYM, continue to copy target data and preserve regular-file metadata. */
	fail_unless(copyfile("link", "copied", 0, LITE_FOPT_KEEP_MTIME) == sizeof(payload));
	fail_unless(lstat("copied", &dst_st) == 0 && S_ISREG(dst_st.st_mode));
	fail_unless(stat("file", &src_st) == 0 && src_st.st_mtime == dst_st.st_mtime);
	fp = fopen("copied", "r");
	fail_unless(fp && fread(buf, 1, sizeof(buf), fp) == sizeof(payload));
	fail_unless(memcmp(buf, payload, sizeof(payload)) == 0 && fclose(fp) == 0);
	fail_unless(unlink("copied") == 0);

	fail_unless(utimensat(AT_FDCWD, "link", times, AT_SYMLINK_NOFOLLOW) == 0);
	fail_unless(copyfile("link", "copied", 0, LITE_FOPT_COPYFILE_SYM | LITE_FOPT_KEEP_MTIME) == 1);
	check_link("copied", "file");
	fail_unless(lstat("copied", &dst_st) == 0 && dst_st.st_mtime == times[1].tv_sec);
	fail_unless(unlink("copied") == 0);
	/* rsync() follows links unless asked to recreate them. */
	fail_unless(rsync("link", "copied", 0, NULL) == 0);
	fail_unless(lstat("copied", &dst_st) == 0 && S_ISREG(dst_st.st_mode));
	fail_unless(dst_st.st_size == sizeof(payload));
	fail_unless(unlink("copied") == 0);
	fail_unless(rsync("link", "copied", LITE_FOPT_RSYNC_LINKS, NULL) == 0);
	check_link("copied", "file");
	/* Reuse the destination after changing the source link. */
	fail_unless(unlink("link") == 0 && symlink("file", "link") == 0);
	fail_unless(rsync("link", "out", LITE_FOPT_RSYNC_LINKS, NULL) == 0);
	check_link("out/link", "file");
	fail_unless(unlink("link") == 0 && symlink("copied", "link") == 0);
	fail_unless(rsync("link", "out", LITE_FOPT_RSYNC_LINKS, NULL) == 0);
	check_link("out/link", "copied");
	/* A same-path copy must not unlink its source. */
	fail_unless(rsync("link", "link", LITE_FOPT_RSYNC_LINKS, NULL) == 0);
	check_link("link", "copied");
	fail_unless(unlink("out/link") == 0);
	/* Replacing a regular destination must also retain the source link. */
	fail_unless(touch("out/link") == 0);
	fail_unless(rsync("link", "out", LITE_FOPT_RSYNC_LINKS, NULL) == 0);
	check_link("out/link", "copied");
	fail_unless(unlink("out/link") == 0);
	/* A failed replacement must keep both the destination and source. */
	fail_unless(mkdir("out/link", 0700) == 0);
	fail_unless(touch("out/link/keep") == 0);
	fail_unless(rsync("link", "out", LITE_FOPT_RSYNC_LINKS, NULL) != 0);
	fail_unless(fexist("out/link/keep"));
	check_link("link", "copied");
	fail_unless(unlink("out/link/keep") == 0 && rmdir("out/link") == 0);
	if (getuid() != 0) {
		fail_unless(mkdir("protected", 0700) == 0);
		fail_unless(symlink("original", "protected/link") == 0);
		fail_unless(chmod("protected", 0500) == 0);
		fail_unless(rsync("link", "protected", LITE_FOPT_RSYNC_LINKS, NULL) != 0);
		check_link("protected/link", "original");
		check_link("link", "copied");
		fail_unless(chmod("protected", 0700) == 0);
		fail_unless(unlink("protected/link") == 0 && rmdir("protected") == 0);
	}
	fail_unless(unlink("copied") == 0 && unlink("link") == 0);
	rsync_stale_links();

	errno = 0;
	fail_unless(copyfile("dir", "copied", 0, LITE_FOPT_COPYFILE_SYM) == 0 && errno == EISDIR);
	errno = 0;
	fail_unless(copyfile("missing", "copied", 0, LITE_FOPT_COPYFILE_SYM) == -1 && errno == ENOENT);

	fail_unless(unlink("file") == 0 && rmdir("dir") == 0 && rmdir("out") == 0);
	fail_unless(chdir(cwd) == 0 && rmdir(tmp) == 0);
	return 0;
}
