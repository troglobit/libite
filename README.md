`-lite`: The missing C-lib functions
====================================

Libite, or `-lite`, is a light-weight library of utility functions and
missing C library functions.  It holds a many functions previously found
in [Finit][1], complemented with a few of my own and some from the
[OpenBSD][2] project, most notably their famous string functions:
[strlcpy(3)][3] and [strlcat(3)][3].  It also holds the very useful *BSD
linked list API [sys/queue.h][4], which is a much more up to date
version than GLIBC carries!  GLIBC does not have the `_SAFE` macros for
traversing lists while deleting/freeing nodes.

Helper Macros
-------------

- `blkdev(dev)`

Create block device

- `chardev(dev)`

Create character device

- `erase(path)`

  Erase file/directory with `remove()`.  Errors on stderr

- `makedir(path)`

  Create directory, like `mkdir()`.  Errors on stderr

- `makefifo(path)`

  Create a FIFO, like `mkfifo()`.  Errors on stderr

- `touch(path)`

  Create a file, or update mtime.  Errors on stderr

- `S_ISEXEC(mode_t m)`

Mysteriously missing from GLIBC

- `UNUSED(var)`

Shorter and more readable version of `var __attribute__ ((unused))`


Generic Functions
-----------------

- `chomp(str)`

  Perl like chomp function, chop off last char if newline.

- `copyfile(src, dst, len, symlink)`

  Like the shell `cp(1)` and `dd(1),` can also create symlinks.

- `dir(dir, ext, filter, list, strip)`

  Wrapper for `scandir()` with optional filter.  Returns a list of
  names: files and directories that must be freed after use.  See
  the unit test at the bottom for an example.

- `fcopyfile(src, dst)`

  Like `copyfile()` but uses already open `FILE *` pointers.  Copies
  from current file positions to current file positions until EOF.

- `fexist(file)`

  Check for the existance of a file, returns True(1) or False(0).

- `fisdir(path)`

  Check for the existance of a directory, returns True(1) or False(0).

- `fmode(file)`

  Returns the `mode_t` bits of a file or directory.

- `fsendfile(src, dst, len)`

  Copy data between file streams, very similar to `fcopyfile()`, but
  `dst` is allowed to be `NULL` to be able to read and discard `len`
  bytes from `src`.

- `makepath(dir)`

  Create all components of the specified directory.

- `mkpath(dir, mode)`

  Like `makepath()`, but also takes a `mode_t` permission mode argument.

- `movefile(src, dst)`

  Like `copyfile()`, but renames `src` to `dst`, or recreates symlink
  with the `dst` name.  On successful operation the source is removed
  and the function returns POSIX OK (0).

- `pidfile(basename)`

  Write a daemon pid file.  Creates a pidfile in `_PATH_VARRUN` using
  either `basename` or, if `basename` is `NULL`, `__progname`.  The
  file name has the form `/var/run/basename.pid`.

  Use this function to create a pid file for your daemon when it is
  ready to receive signals.  A client application may poll for the
  existence of this file, so make sure to have your signal handlers
  properly setup before calling this function.
  
  The pidfile is removed when the program exits, using an `atexit()`
  handler.  However, depending on how the program terminates the file
  may still exist even though the program is no longer running.  This
  is only a problem for clients.

- `rsync(src, dst, delete, *filter())`

  Very simple `rsync()` to copy files files and directories
  recursively.

- `tempfile()`

  Secure replacement for `tmpfile()`.  Creates an invisible temporary
  file in `/tmp` that is removed when the returned `FILE` pointer is
  closed.  **Note:** Requires Linux v3.11, or later.


OpenBSD Functions
-----------------

The following are the popular OpenBSD string functions.

- `strlcpy(dst, src, len)`
- `strlcat(dst, src, len)`
- `strtonum()`
- [`sys/queue.h`][4] API


TODO
----

- Improve documentation, possibly use kdoc or gdoc2 to generate docs from API.

[1]: https://github.com/troglobit/finit
[2]: http://www.openbsd.org/
[3]: http://www.openbsd.org/cgi-bin/man.cgi?query=strlcpy
[4]: http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/LIST_EMPTY.3

<!--
  -- Local Variables:
  -- mode: markdown
  -- End:
  -->
