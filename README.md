Light-weight library of missing C library functions
===================================================

Libite, or `-lite`, is a light-weight library of utility functions and
missing C library functions.


Generic Functions
-----------------

- `dir()`
- `rsync(src, dst, delete, *filter())`
- `copyfile(src, dst, len, symlink)`
- `fexist(file)`
- `fisdir(path)`
- `fmode(file)`


OpenBSD Functions
-----------------

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
