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

#include <err.h>
#include <stdio.h>
#include <stdint.h>    /* uint8_t, uint16_t, uint32_t, INT32_MAX, etc. */
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h> /* MAX(), isset(), setbit(), TRUE, FALSE, et consortes. :-) */
#include <unistd.h>

typedef struct lfile lfile_t;

char   *chomp      (char *str);

int     fexist     (char *file);
int     fisdir     (char *file);
mode_t  fmode      (char *file);

FILE   *tempfile   (void);
ssize_t copyfile   (char *src, char *dst, int len, int sym);
int     movefile   (char *src, char *dst);
int     fcopyfile  (FILE *src, FILE *dst);
size_t  fsendfile  (FILE *src, FILE *dst, size_t len);

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
int     rsync      (char *src, char *dst, int delete, int (*filter) (const char *file));

int     pidfile       (const char *basename);
int     pidfile_signal(const char *pidfile, int signal);
pid_t   pidfile_read  (const char *pidfile);
pid_t   pidfile_poll  (const char *pidfile);

#ifndef strlcpy
size_t  strlcpy    (char *dst, const char *src, size_t siz);
#endif
#ifndef strlcat
size_t  strlcat    (char *dst, const char *src, size_t siz);
#endif
#ifndef strtonum
long long strtonum (const char *numstr, long long minval, long long maxval, const char **errstrp);
#endif

int tree(char *path, int show_perms);

#ifndef touch
# define touch(x) do { if (mknod((x), S_IFREG|0644, 0) && errno != EEXIST) warn("Failed creating %s", x); } while (0)
#endif
#ifndef makedir
# define makedir(x, p) do { if (mkdir(x, p) && errno != EEXIST) warn("Failed creating directory %s", x); } while (0)
#endif
#ifndef makefifo
# define makefifo(x, p) do { if (mkfifo(x, p) && errno != EEXIST) warn("Failed creating FIFO %s", x); } while (0)
#endif
#ifndef erase
# define erase(x) do { if (remove(x) && errno != ENOENT) warn("Failed removing %s", x); } while (0)
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

/* Convert string to natural number (0-2147483647), returns -1 on error. */
static inline int atonum(const char *str)
{
	int val = -1;
	const char *errstr;

	if (str) {
		val = strtonum(str, 0, INT32_MAX, &errstr);
		if (errstr)
			return -1;
	}

	return val;
}

/* Validate string, non NULL and not zero length */
static inline int string_valid(const char *s)
{
   return s && strlen(s);
}

/* Relaxed comparison, e.g., sys_string_match("small", "smaller") => TRUE */
static inline int string_match(const char *a, const char *b)
{
   size_t min = MIN(strlen(a), strlen(b));

   return !strncasecmp(a, b, min);
}

/* Strict comparison, e.g., sys_string_match("small", "smaller") => FALSE */
static inline int string_compare (const char *a, const char *b)
{
   return strlen(a) == strlen(b) && !strcmp(a, b);
}

/* Strict comparison, like sys_string_compare(), but case insensitive,
 * e.g., sys_string_match("small", "SmAlL") => TRUE
 */
static inline int string_case_compare (const char *a, const char *b)
{
   return strlen (a) == strlen (b) && !strcasecmp (a, b);
}

#define min(a,b)				\
	({					\
		__typeof__ (a) _a = (a);	\
		__typeof__ (b) _b = (b);	\
		_a < _b ? _a : _b;		\
	})

#define max(a,b)				\
	({					\
		__typeof__ (a) _a = (a);	\
		__typeof__ (b) _b = (b);	\
		_a > _b ? _a : _b;		\
	})

/* Compat */
#define copy_filep(src,dst)        fcopyfile(src,dst)
#define pidfile_read_pid(file)     pifile_read(file)
#define signal_pidfile(file,signo) pidfile_signal(file,signo)

#endif /* LITE_H_ */

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */

