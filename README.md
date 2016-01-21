-lite | Frog DNA, basically
===========================
[![Travis Status][]][Travis]

Libite is a lightweight library of *frog DNA*.  It can be used to fill
the gaps in any dinosaur project.  It holds useful functions and macros
developed by both [Finit][1] and the [OpenBSD][2] project.  Most notably
the string functions: [strlcpy(3)][3], [strlcat(3)][3] and the highly
useful *BSD [sys/queue.h][4] and [sys/tree.h][7] API's.

Libite aims to fill in the gaps missing in GLIBC/EGLIBC.  (It does not
aimo to become another [GLIB][5] though.)  One such gap in GLIBC is the
missing `_SAFE` macros in `sys/queue.h` &mdash; highly recommended when
traversing lists to delete/free nodes.

The code is open sourced under a mix of the [MIT/X11 license][MIT], the
[ISC license][ISC] used by OpenBSD, and [BSD licenses][BSD], all of them
are extremely liberal and can be used freely in proprietary software if
needed.

For an introduction to why Libite happened, and how you can use it, see
[this blog post][6].


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

  Erase file/directory with `remove()`.  Errors on stderr

- `makedir(path)`

  Create directory, like `mkdir()`.  Errors on stderr

- `makefifo(path)`

  Create a FIFO, like `mkfifo()`.  Errors on stderr

- `min(a,b)`/`max(a,b)`

  These macros take care to avoid double evaluation.

- `touch(path)`

  Create a file, or update mtime.  Errors on stderr

- `S_ISEXEC(mode_t m)`

  Mysteriously missing from GLIBC

- `ISCLR(word,bit)`

  Is bit in (integer) word cleared?

- `ISSET(word,bit)`

  Is bit in (integer) word set?

- `ISOTHER(word,bit)`

  Is any other bits, except bit, in (integer) word set?

- `SETBIT(word,bit)`

  Set bit in (integer) word.

- `CLRBIT(word,bit)`

  Clear bit in (integer) word.

- `NELEMS(array)`

  Returns the number of elements in an array.  From the great book, The
  Practice of Programming, by Kernighan and Pike.

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

- `ifconfig(ifname, addr, mask, up)`

  Basic ifconfig like operations on an interface.  Only supports IPv4
  adresses.  Note that mask is not CIDR notation.

- `lfopen(file, sep)`, `lfclose(lf)`

  API for parsing UNIX style configuration files like `/etc/protocols`
  and `/etc/services`.

- `lftok(lf)`

  Read tokens, delimeted by `sep`, from file opened with `lfopen()`.

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

- `movefile(src, dst)`

  Like `copyfile()`, but renames `src` to `dst`, or recreates symlink
  with the `dst` name.  On successful operation the source is removed
  and the function returns POSIX OK (0).

- `pidfile(basename)`

  Create a daemon PID file in `_PATH_VARRUN` using either `basename` or,
  if `basename` is `NULL`, `__progname`.  The resulting file name is
  available to the user as a read-only pointer:

        extern char *__pidfile_name;

  Use this function to create a PID file for your daemon when it is
  ready to receive signals.  A client application may poll for the
  existence of this file, so make sure to have your signal handlers
  properly setup before calling this function.

  The pidfile is removed when the program exits, using an `atexit()`
  handler.  However, depending on how the program terminates the file
  may still exist even though the program is no longer running.  This
  is only a problem for clients.

  Calling this function multiple times updates the mtime of the file.
  Only one `atexit()` handler is created, regardless of the amount of
  times the function is called.
  
  See below for link to OpenBSD man page.

- `pidfile_read(pidfile)`

  Read PID from pid file created by `pidfile()`.

- `pidfile_signal(pidfile, signal)`

  Send signal to PID found in pid file created by `pidfile()`.

- `progress(percent, max_width)`

  Simple ASCII progress bar with a spinner.  Start it with `percent=0`
  and set the `max_width=chars` to indicate width of the progress bar.
  Called multiple times with the same percentage value cause spinner to
  spin.

- `rsync(src, dst, delete, *filter())`

  Very simple `rsync()` to copy files files and directories
  recursively.

- `tempfile()`

  Secure replacement for `tmpfile()`.  Creates an invisible temporary
  file in `/tmp` that is removed when the returned `FILE` pointer is
  closed.
  
  **Note:** Requires Linux v3.11, or later, will fall back to the old
    and unsafe `tmpfile()` on older systems.

- `tree(path, show_perms)`

  Very simple `/bin/tree` replacement.  Draw ASCII tree of `path`, with
  optional listing of file and directory permissions if `show_perms` is
  set.  The `path` argument should be a directory.


OpenBSD Functions
-----------------

The following are popular OpenBSD functions and highly useful macros.

- `pidfile(basename)`

  http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/pidfile.3

  **Note:** this version of `pidfile()` has been extended to handle it
  being called multiple times, and also to export the path to the PID
  file `__pidfile_name`, similar to `__progname`.  See previous section
  for details.

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


Build & Install
---------------

The library is built for and developed on GNU/Linux systems as a light
weight utility library.  Libite (LITE) employs the de facto standard GNU
configure and build system:

    ./configure
    make all
    make install


TODO
----

- Improve documentation, possibly use kdoc or gdoc2 to generate docs from API.
- Continuously, update OpenBSD functions/macros.

[1]: https://github.com/troglobit/finit
[2]: http://www.openbsd.org/
[3]: http://www.openbsd.org/cgi-bin/man.cgi?query=strlcpy
[4]: http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/LIST_EMPTY.3
[5]: https://developer.gnome.org/glib/
[6]: http://troglobit.com/blog/2015/07/02/howto-using-lite-with-a-git-based-application/
[7]: http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/SPLAY_FOREACH.3
[MIT]: https://en.wikipedia.org/wiki/MIT_License
[ISC]: https://en.wikipedia.org/wiki/ISC_license
[BSD]: https://en.wikipedia.org/wiki/BSD_licenses
[Travis]: https://travis-ci.org/troglobit/libite
[Travis Status]: https://travis-ci.org/troglobit/libite.png?branch=master

<!--
  -- Local Variables:
  -- mode: markdown
  -- End:
  -->
