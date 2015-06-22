`-lite`: The missing C-lib functions
====================================

Libite, or `-lite`, is a light-weight library of utility functions and
missing C library functions.

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

- `fexist(file)`

  Check for the existance of a file, returns True(1) or False(0).

- `fisdir(path)`

  Check for the existance of a directory, returns True(1) or False(0).

- `fmode(file)`

  Returns the `mode_t` bits of a file or directory.

- `rsync(src, dst, delete, *filter())`

  Very simple `rsync()` to copy files files and directories
  recursively.

- `tempfile()`

  Secure replacement for `tmpfile()`.  Creates an invisible temporary
  file in `/tmp` that is removed when the returned %FILE pointer is
  closed.


OpenBSD Functions
-----------------

The following are the popular OpenBSD string functions.

- `strlcpy(dst, src, len)`
- `strlcat(dst, src, len)`
- `strtonum()`


TODO
----

- Improve documentation, possibly use kdoc or gdoc2 to generate docs from API.


<!--
  -- Local Variables:
  -- mode: markdown
  -- End:
  -->
