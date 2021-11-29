-lite | Frog DNA, basically
===========================
[![GitHub Status][]][GitHub] [![codedocs status][]][codedocs] [![Coverity Status][]][Coverity Scan]

Table of Contents
-----------------

* [Introduction](#introduction)
* [Using -lite](#using--lite)
* [API](doc/API.md#overview)
  * [Important Node](doc/API.md#important-note)
  * [Helper Macros](doc/API.md#helper-macros)
  * [Generic Functions](doc/API.md#generic-functions)
  * [GNU Functions](doc/API.md#gnu-functions)
  * [OpenBSD/NetBSD/FreeBSD Functions](doc/API.md#openbsd-netbsd-freebsd-functions)
* [Build & Install](#build--install)
* [Origin & References](#origin--references)


> **NOTE:** Incompatible changes in v2.0 compared to v1.x!

Introduction
------------

Libite is a lightweight library of *frog DNA* that can be used to fill
the gaps in any dinosaur project.  It holds useful functions and macros
developed by both [Finit][1] and the [OpenBSD][2] project.  Most notably
the string functions: [strlcpy(3)][3], [strlcat(3)][3] and the highly
useful *BSD [sys/queue.h][4] and [sys/tree.h][7] API's.

Libite holds many of the missing pieces in GNU libc, although -lite does
not aim to become another [GLIB][5].  One noticeable gap in GLIBC is the
`_SAFE` macros available in the BSD `sys/queue.h` API &mdash; highly
recommended when traversing lists to delete/free nodes.

The code is open sourced under a mix of permissive Open Source licenses:
[MIT/X11 license][MIT], [ISC license][ISC], and [BSD licenses][BSD].

> **Tip:** have a look at my blog post about other useful UNIX API's!
>          https://troglobit.com/post/2020-02-22-useful-unix-apis/


Using -lite
-----------

Libite is by default installed as a library and a set of include files.
To prevent clashing with include files of the same name `-lite` employs
an include file namespace `libite/`, which is strongly recommended to
use in your applications:

```C
#include <libite/lite.h>
#include <libite/conio.h>
#include <libite/queue.h>
#include <libite/tree.h>
```

> **Note:** prior to v2.5.0, the `lite/` namespace was used for headers,
> which is still available in the default install.  This clashed with
> the headers of the LiTE library from the DirectFB project.

The output from the `pkg-config` tool holds no surprises:

```bash
$ pkg-config --libs --static --cflags libite
-I/usr/local/include -D_LIBITE_LITE -L/usr/local/lib -lite
```

> **Note:** `_LIBITE_LITE` is defined since v2.5.0, useful for software
> that want to be able to build against headers from an older libite:
>
> ```C
> #ifdef _LIBITE_LITE
> # include <libite/lite.h>
> #else
> # include <lite/lite.h>
> #endif
> ```

The prefix path `/usr/local/` shown here is only the default.  Use the
`configure` script to select a different prefix when installing libite.

For GNU autotools based projects, use the following in `configure.ac`:

```m4
# Check for required libraries
PKG_CHECK_MODULES([lite], [libite >= 1.5.0])
```

and in your `Makefile.am`:

```Makefile
proggy_CFLAGS = $(lite_CFLAGS)
proggy_LDADD  = $(lite_LIBS)
```

> API Documentaion: https://codedocs.xyz/troglobit/libite/


Build & Install
---------------

This library was initially built for and developed on GNU/Linux systems
as a light weight utility library, these days NetBSD should also work.

```bash
$ ./configure
$ make -j5
$ sudo make install-strip
$ sudo ldconfig
```

Use <kbd>./configure --without-symlink</kbd> to prevent the install step
from creating the `lite -> libite/` compatibility symlink for the header
files, required for systems with DirectFB LiTE.  The default, however,
is to install the symlink to ensure compatibility with existing software
that depends on the `-lite` library headers in their previous namespace.

**Note:** When checking out code from GIT, use <kbd>./autogen.sh</kbd>
to generate a `configure` script.  It is a generated file and otherwise
only included in released tarballs.


Origin & References
-------------------

Much of the code in libite (-lite) is written by [Claudio Matsuoka][]
for the original [Finit][original finit] and released under the MIT/X11
license.  Joachim Wiberg later improved on the [Finit][1] code base and
included pieces of software released under the ISC and BSD licenses.
See each respective file for license details.

[1]:                https://github.com/troglobit/finit
[2]:                http://www.openbsd.org/
[3]:                http://www.openbsd.org/cgi-bin/man.cgi?query=strlcpy
[4]:                http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/LIST_EMPTY.3
[5]:                https://developer.gnome.org/glib/
[7]:                http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/SPLAY_FOREACH.3
[MIT]:              https://en.wikipedia.org/wiki/MIT_License
[ISC]:              https://en.wikipedia.org/wiki/ISC_license
[BSD]:              https://en.wikipedia.org/wiki/BSD_licenses
[GitHub]:           https://github.com/troglobit/libite/actions/workflows/build.yml/
[GitHub Status]:    https://github.com/troglobit/libite/actions/workflows/build.yml/badge.svg
[Coverity Scan]:    https://scan.coverity.com/projects/20602
[Coverity Status]:  https://img.shields.io/coverity/scan/20602.svg
[codedocs]:         https://codedocs.xyz/troglobit/libite.log
[codedocs status]:  https://codedocs.xyz/troglobit/libite.svg
[Claudio Matsuoka]: https://github.com/cmatsuoka
[original finit]:   http://helllabs.org/finit/
