`-lite`: A compilation of missing C-lib functions
=================================================

Libite, or `-lite`, is a light-weight library of utility functions and
missing C library functions.


Generic Functions
-----------------

- `dir(dir, ext, filter, list, strip)`

  Wrapper for `scandir()` with optional filter.  Returns a list of
  names: files and directories that must be freed after use.  See
  the unit test at the bottom for an example.

- `rsync(src, dst, delete, *filter())`

  Very simple `rsync()` to copy files files and directories
  recursively.

- `copyfile(src, dst, len, symlink)`

  Like the shell `cp(1)` and `dd(1),` can also create symlinks.

- `fexist(file)`

  Check for the existance of a file, returns True(1) or False(0).

- `fisdir(path)`

  Check for the existance of a directory, returns True(1) or False(0).

- `fmode(file)`

  Returns the `mode_t` bits of a file or directory.


OpenBSD Functions
-----------------

The following are the popular OpenBSD string functions.

- `strlcpy(dst, src, len)`
- `strlcat(dst, src, len)`
- `strtonum()`


TODO
----

- Improve documentation, possibly use kdoc or gdoc2 to generate docs from API.
- Add `chomp()` from Finit


<!--
  -- Local Variables:
  -- mode: markdown
  -- End:
  -->
