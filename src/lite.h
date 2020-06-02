/* Collection of frog DNA
 *
 * Copyright (c) 2008-2010  Claudio Matsuoka <cmatsuoka@gmail.com>
 * Copyright (c) 2008-2020  Joachim Nilsson <troglobit@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LITE_H_
#define LITE_H_

#include <errno.h>
#include <fcntl.h>     /* Definition of AT_* constants for utimensat() */
#include <stdio.h>
#include <stdint.h>    /* uint8_t, uint16_t, uint32_t, INT32_MAX, etc. */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h> /* MAX(), isset(), setbit(), TRUE, FALSE, et consortes. :-) */
#include <unistd.h>
#include "strlite.h"

/*
 * fparseln() specific operation flags.
 */
#define FPARSELN_UNESCESC	0x01
#define FPARSELN_UNESCCONT	0x02
#define FPARSELN_UNESCCOMM	0x04
#define FPARSELN_UNESCREST	0x08
#define FPARSELN_UNESCALL	0x0f

/*
 * copyfile() and rsync() option flags
 */
#define LITE_FOPT_RSYNC_DELETE  0x01
#define LITE_FOPT_COPYFILE_SYM  0x01
#define LITE_FOPT_KEEP_MTIME    0x02

typedef struct lfile lfile_t;
typedef struct sdbuf sdbuf_t;

char   *chomp      (char *str);
char   *fparseln   (FILE *, size_t *, size_t *, const char[3], int);

int     fexist     (const char *file);
int     fisdir     (const char *path);

FILE   *fopenf     (const char *mode, const char *fmt, ...);
int     fremove    (const char *fmt, ...);
FILE   *tempfile   (void);
ssize_t copyfile   (const char *src, const char *dst, int len, int opt);
int     movefile   (const char *src, const char *dst);
int     fcopyfile  (FILE *src, FILE *dst);
ssize_t fsendfile  (FILE *src, FILE *dst, size_t len);
int     truncatef  (off_t length, const char *fmt, ...);

int     ifconfig   (const char *ifname, const char *addr, const char *mask, int up);

lfile_t*lfopen     (const char *file, const char *sep);
void    lfclose    (lfile_t *lf);
char   *lftok      (lfile_t *lf);
char   *lfgetkey   (lfile_t *lf, const char *key);
int     lfgetint   (lfile_t *lf, const char *key);
int     fgetint    (const char *file, const char *sep, const char *key);

int     fmkpath    (mode_t mode, const char *fmt, ...);
int     mkpath     (const char *dir, mode_t mode);
int     makepath   (const char *dir);

int     dir        (const char *dir, const char *type, int (*filter) (const char *file), char ***list, int strip);
int     rsync      (char *src, char *dst, int opt,  int (*filter) (const char *file));

int     pidfile       (const char *basename);
int     pidfile_signal(const char *pidfile, int signal);
pid_t   pidfile_read  (const char *pidfile);
pid_t   pidfile_poll  (const char *pidfile);

void   *reallocarray(void *optr, size_t nmemb, size_t size);

void    progress       (int percent, int max_width);
void    progress_simple(int percent);

int     systemf        (const char *fmt, ...);

sdbuf_t*telnet_open    (int addr, short port);
int     telnet_close   (sdbuf_t *ctx);
int     telnet_expect  (sdbuf_t *ctx, char *script[], FILE *output);
int     telnet_session (int addr, short port, char *script[], FILE *output);

int     yorn       (const char *fmt, ...);

char   *which      (const char *cmd);
int     whichp     (const char *cmd);

#ifndef touch
#include <sys/stat.h>		/* utimensat() */
#include <sys/time.h>		/* utimensat() on *BSD */
static inline int touch(const char *path)
{
	if (mknod((path), S_IFREG|0644, 0)) {
		if (errno == EEXIST)
			return utimensat(AT_FDCWD, path, NULL, 0);
		return -1;
	}
	return 0;
}
#endif
#ifndef makedir
static inline int makedir(const char *path, mode_t mode)
{
	if (mkdir(path, mode) && errno != EEXIST)
		return -1;
	return 0;
}
#endif
#ifndef makefifo
static inline int makefifo(const char *path, mode_t mode)
{
	if (mkfifo(path, mode) && errno != EEXIST)
		return -1;
	return 0;
}
#endif
#ifndef erase
static inline int erase(const char *path)
{
	if (remove(path) && errno != ENOENT)
		return -1;
	return 0;
}
#endif

#ifndef chardev
# define chardev(x,m,maj,min) mknod((x), S_IFCHR|(m), makedev((maj),(min)))
#endif
#ifndef blkdev
# define blkdev(x,m,maj,min) mknod((x), S_IFBLK|(m), makedev((maj),(min)))
#endif

#ifndef S_ISEXEC
# define S_ISEXEC(m) (((m) & S_IXUSR) == S_IXUSR)
#endif

/* Unline isset(), setbit() et al, these work with integers/shorts/longwords/etc. */
#ifndef ISCLR
#define ISCLR(word,bit)   ((word &   (1 << (bit)) ? 0 : 1))
#endif
#ifndef ISSET
#define ISSET(word,bit)   ((word &   (1 << (bit)) ? 1 : 0))
#endif
#ifndef ISOTHER
#define ISOTHER(word,bit) ((word &  ~(1 << (bit)) ? 1 : 0)) /* Is any other bit set? */
#endif
#ifndef SETBIT
#define SETBIT(word,bit)   (word |=  (1 << (bit)))
#endif
#ifndef CLRBIT
#define CLRBIT(word,bit)   (word &= ~(1 << (bit)))
#endif

/* From The Practice of Programming, by Kernighan and Pike */
#ifndef NELEMS
#define NELEMS(array) (sizeof(array) / sizeof(array[0]))
#endif

/* Does directory end with a slash? */
static inline int fisslashdir(const char *dir)
{
   if (!dir)
	   return 0;

   if (strlen(dir) > 0)
	   return dir[strlen(dir) - 1] == '/';

   return 0;
}

/* Compat */
#define copy_filep(src, dst)        fcopyfile(src, dst)
#define pidfile_read_pid(file)      pifile_read(file)
#define signal_pidfile(file, signo) pidfile_signal(file, signo)

#endif /* LITE_H_ */

#ifdef __cplusplus
}
#endif

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */

