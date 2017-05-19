/* Collection of frog DNA
 *
 * Copyright (c) 2008-2010  Claudio Matsuoka <cmatsuoka@gmail.com>
 * Copyright (c) 2008-2016  Joachim Nilsson <troglobit@gmail.com>
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

#ifndef LITE_H_
#define LITE_H_

#include <errno.h>
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

typedef struct lfile lfile_t;

char   *chomp      (char *str);
char   *fparseln   (FILE *, size_t *, size_t *, const char[3], int);

int     fexist     (char *file);
int     fisdir     (const char *file);
mode_t  fmode      (char *file);

FILE   *tempfile   (void);
ssize_t copyfile   (char *src, char *dst, int len, int sym);
int     movefile   (char *src, char *dst);
int     fcopyfile  (FILE *src, FILE *dst);
ssize_t fsendfile  (FILE *src, FILE *dst, size_t len);

int     ifconfig   (char *ifname, char *addr, char *mask, int up);

lfile_t*lfopen     (char *file, char *sep);
void    lfclose    (lfile_t *lf);
char   *lftok      (lfile_t *lf);
char   *lfgetkey   (lfile_t *lf, char *key);
int     lfgetint   (lfile_t *lf, char *key);
int     fgetint    (char *file, char *sep, char *key);

int     mkpath     (char *dir, mode_t mode);
int     makepath   (char *dir);

int     dir        (const char *dir, const char *type, int (*filter) (const char *file), char ***list, int strip);
int     rsync      (char *src, char *dst, int delete,  int (*filter) (const char *file));

int     pidfile       (const char *basename);
int     pidfile_signal(const char *pidfile, int signal);
pid_t   pidfile_read  (const char *pidfile);
pid_t   pidfile_poll  (const char *pidfile);

void   *reallocarray(void *optr, size_t nmemb, size_t size);

int     tree(char *path, int show_perms);

void    progress       (int percent, int max_width);
void    progress_simple(int percent);

#ifndef touch
#include <sys/stat.h>		/* utimensat() */
#include <sys/time.h>		/* utimensat() on *BSD */
static inline int touch(const char *path)
{
	if (mknod((path), S_IFREG|0644, 0)) {
		if (errno == EEXIST)
			return utimensat(0, path, NULL, 0);
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

/* Mark a function variable as unused, useful for generic callbacks */
#ifndef UNUSED
#define UNUSED(x) UNUSED_ ## x __attribute__ ((unused))
#endif

/* Does directory end with a slash? */
static inline int fisslashdir(char *dir)
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

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */

