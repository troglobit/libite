API Documentation
=================


Important Note
--------------

When using functions like `mkpath()`, `makepath()`, and `makefifo()`,
make sure your `umask()` is correct so you do not accidentally create
files and directories accessible to other users than intended.


Helper Macros
-------------

- `atonum(str)`

  Convert string to natural number, works for 32-bit non-negative
  integers.  Returns -1 on error.  (Uses `strtonum()` internally.)

- `blkdev(dev)`

  Create block device

- `chardev(dev)`

  Create character device

- `erase(path)`

  Erase file/directory with `remove()`.  Errors on `stderr`

- `erasef(fmt, ...)`

  Like `erase()` but takes a formatted printf-like string as argument.

- `makedir(path)`

  Create directory, like `mkdir()`.  Errors on `stderr`

- `makefifo(path)`

  Create a FIFO, like `mkfifo()`.  Errors on `stderr`

- `min(a,b)`/`max(a,b)`

  These macros take care to avoid double evaluation.

- `touch(path)`

  Create a file, or update mtime.  Errors on `stderr`

- `touchf(fmt, ...)`

  Like `touch()` but takes a formatted printf-like string as argument.

- `S_ISEXEC(mode_t m)`

  Mysteriously missing from GLIBC

- `ISCLR(word,bit)`

  Is bit in (integer) word cleared?

- `ISSET(word,bit)`

  Is bit in (integer) word set?

- `ISOTHER(word,bit)`

  Are any other bits, except bit, in (integer) word set?

- `SETBIT(word,bit)`

  Set bit in (integer) word.

- `CLRBIT(word,bit)`

  Clear bit in (integer) word.

- `NELEMS(array)`

  Returns the number of elements in an array.  From the great book, The
  Practice of Programming, by Kernighan and Pike.


Generic Functions
-----------------

- `chomp(str)`

  Perl like chomp function, chop off last char if newline.

- `copyfile(src, dst, len, opt)`

  Like the shell `cp(1)` and `dd(1),`, can also copy symlinks and
  preserve the mtime of the source file.  The opt argument can be
  a mask of:
  
  - `LITE_FOPT_COPYFILE_SYM (0x01)`
  - `LITE_FOPT_KEEP_MTIME   (0x02)`
  
  In releases prior to v2.0 the opt argument was called `symlink`.
  The APIs are 100% compatible if the value `1` was used to enable
  the symlink option.

- `dir(dir, ext, filter, list, strip)`

  Wrapper for `scandir()` with optional filter.  Returns a list of
  names: files and directories that must be freed after use.  See
  the unit test for an example, or take a look at `glob(3)`, it's
  probably what you want anyway.

- `fcopyfile(src, dst)`

  Like `copyfile()` but uses already open `FILE *` pointers.  Copies
  from current file positions to current file positions until EOF.

- `fexist(file)`

  Check for the existence of a file, returns True(1) or False(0).

- `fisdir(path)`

  Check for the existence of a directory, returns True(1) or False(0).

- `fopenf(mode, fmt, ...)`

  Like `fopen()`, but takes a formatted string as argument.  This
  greatly simplifies operations that usually consist of composing a
  filename from parts into a dynamic buffer before actually opening
  the file.
  
  > **Notice:** the swapped order of `pathname` and `mode`!

- `fremove(fmt, ...)`

  Like `remove()`, but takes a formatted string as argument.  This
  greatly simplifies operations that usually consist of composing a
  filename from parts into a dynamic buffer before actually removing
  the file.

- `fsendfile(src, dst, len)`

  Copy data between file streams, very similar to `fcopyfile()`, but
  `dst` is allowed to be `NULL` to be able to read and discard `len`
  bytes from `src`.

- `ifconfig(ifname, addr, mask, up)`

  Basic ifconfig like operations on an interface.  Only supports IPv4
  addresses.  Note that mask is not CIDR notation.

- `lfopen(file, sep)`, `lfclose(lf)`

  LITE file API for parsing UNIX style configuration files like
  `/etc/protocols` and `/etc/services`.

- `lftok(lf)`

  Read tokens, delimited by `sep`, from file opened with `lfopen()`.

- `lfgetkey(lf, key)`

  Find `key` in file opened with `lfopen()`, return value/argument.

- `lfgetint(lf, key)`

  Wrapper for `lfgetkey()`, returns positive integer value to `key`,
  or `-1` if `key` is not found.

- `fgetint(file, sep, key)`

  Wrapper for `lfopen()`, `lfgetint()`, and `lfclose()`.  Useful for
  when only reading a single `key` from a file.

- `makepath(dir)`

  Create all components of the specified directory.

- `mkpath(dir, mode)`

  Like `makepath()`, but also takes a `mode_t` permission mode argument.

- `fmkpath(mode, fmt, ...)`

  Like `mkpath()`, but takes a formatted string as argument.

  > **Notice:** the swapped order of `pathname` and `mode`!

- `movefile(src, dst)`

  Like `copyfile()`, but renames `src` to `dst`, or recreates symlink
  with the `dst` name.  On successful operation the source is removed
  and the function returns POSIX OK (0).

- `pidfile(name)`

  Create a daemon PID file using either the `name` as a full path, if
  `name` starts with `/`, or in `_PATH_VARRUN` using `name` as the
  basename of the application.  If `name` is `NULL`, then `__progname`
  is used as the basename.  The resulting file name is available to the
  user as a read-only pointer:

        extern char *__pidfile_name;

  Use this function to create a PID file for your daemon when it is
  ready to receive signals.  A client application may poll for the
  existence of this file, so make sure to have your signal handlers
  properly setup before calling this function.

  The PID file is removed when the program exits, using an `atexit()`
  handler.  However, depending on how the program terminates the file
  may still exist even though the program is no longer running.

  Calling this function multiple times updates the mtime of the file.
  Only one `atexit()` handler is created, regardless of the amount of
  times the function is called.  If the file is removed, subsequent
  calls to this function will recreate the file.
  
  See below for a link to OpenBSD man page.

- `pidfile_read(pidfile)`

  Read PID from pid file created by `pidfile()`.

- `pidfile_signal(pidfile, signal)`

  Send signal to PID found in pid file created by `pidfile()`.

- `progress(percent, max_width)`

  Simple ASCII progress bar with a spinner.  Start it with `percent=0`
  and set the `max_width=chars` to indicate width of the progress bar.
  Called multiple times with the same percentage value cause spinner to
  spin.

- `rsync(src, dst, opt, *filter())`

  Very simple `rsync()` to copy files and directories recursively.  It
  supports pruning files from the destination tree that do not exist in
  the source tree and preserving the mtime of the source files.  The opt
  argument can be a mask of:
  
  - `LITE_FOPT_RSYNC_DELETE (0x01)`
  - `LITE_FOPT_KEEP_MTIME   (0x02)`
  
  In releases prior to v2.0 the argument controlling pruning was called
  `delete`, it is now called `opt`.  The APIs are 100% compatible if the
  value `1` was used.

- `strmatch(str, list)`, `strnmatch(str, list, len)`

  Find matching string in an array of strings.  Returns index in array on
  match, or `-1` on error or not found.  `strnmatch()` takes an extra arg
  to compare only `len` number of characters from `str`.

- `strtrim(str)`

  Trims a string from any leading and trailing white-space, returns the
  trimmed result in the same buffer.

- `systemf(fmt, ...)`

  Like `system()`, but takes a formatted string as argument.  This
  greatly simplifies operations that usually consist of composing a
  command from parts into a dynamic buffer before calling it.

- `telnet_open(addr, port), telnet_close(ctx), telnet_expect(ctx, script, output)`

  Poor mans telnet expect in C.  Opens connection to a Telnet service;
  FTP, Telnet, similar, and run an expect-like script.

- `telnet_session(addr, port, script output)`

  Wrapper for the above three in one API.

- `tempfile()`

  Secure replacement for `tmpfile()`.  Creates an invisible temporary
  file in `/tmp` that is removed when the returned `FILE` pointer is
  closed.
  
  **Note:** Requires Linux v3.11, or later, will fall back to the old
    and unsafe `tmpfile()` on older systems.

- `truncatef(length, fmt, ...)`

  Like `truncate()`, but takes a formatted string as argument.  This
  simplifies some operations when the filename otherwise have to be
  composed from parts into a separate array before calling the real
  function.

- `which(cmd)`, `whichp(cmd)`

  C implementation of UNIX which(1).  Returns a malloc'ed string with the
  full path to `cmd` on success, otherwise `NULL`.
  
  `whichp()` is a predicate function, returns `TRUE` or `FALSE`.

  **Note:** `which("/bin/ps aux")` will return `/bin/ps`, or `TRUE`,
  provided of course `/bin/ps` exists.

- `yorn(fmt, ...)`

  Pose a question to user, appended with `(y/N)?`, returns `TRUE` for
  yes (both `y` and `Y` are handled) and `FALSE` for everything else.


GNU Functions
-------------

The following are useful GNU functions, that do not exist on *BSD, and
some other platforms.

- `strdupa(str)`

  http://man7.org/linux/man-pages/man3/strdupa.3.html

- `strndupa(str)`

  http://man7.org/linux/man-pages/man3/strndupa.3.html

- `strnlen(str, lim)`

  http://man7.org/linux/man-pages/man3/strnlen.3.html


OpenBSD/NetBSD/FreeBSD Functions
--------------------------------

The following are popular functions and highly useful macros from the
*BSD world of UNIX that are sadly missing on Linux.

- `fparseln(fp, *len, *lineno, delim, flags)`

  http://man.openbsd.org/OpenBSD-current/man3/fparseln.3

- `pidfile(basename)`

  http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/pidfile.3

  **Note:** the version of `pidfile()` in -lite has been extended to
  handle it being called multiple times, i.e. to not create multiple
  `atexit()` handlers, and also to export a few internal paths:

	- `__pidfile_path`: prefix path, default `_PATH_VARRUN`.  Notice the
	  trailing slash requirement if you want to override it!
	- `__pidfile_name`: full path to PID file , similar to `__progname`.
	  See previous section for details.

- `reallocarray(ptr, nmemb, size)`

  http://man.openbsd.org/OpenBSD-current/man3/reallocarray.3

- `strlcpy(dst, src, len)`

  http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/strlcpy.3

- `strlcat(dst, src, len)`

  http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/strlcat.3

- `strtonum()`

  http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/strtonum.3

- `sys/queue.h` API

  http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/LIST_EMPTY.3

- `sys/tree.h` API

  Niels Provos' famous splay and red-black tree implementation.

  http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/SPLAY_FOREACH.3
