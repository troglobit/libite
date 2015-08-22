.PHONY: all clean distclean

# Figure out root of library, unless used as submodule
ROOTDIR    ?= $(shell pwd)

CC         ?= $(CROSS)gcc
AR         ?= $(CROSS)ar
STRIP      ?= $(CROSS)strip
INSTALL    := install --backup=off
STRIPINST  := $(INSTALL) -s --strip-program=$(CROSS)strip -m 0755

CPPFLAGS   += -D_GNU_SOURCE
ARFLAGS     = crus

# Default to silent build, use V=1 to get verbose mode.
ifeq ($V,1)
Q           =
PRINT       = @true
REDIRECT    =
MAKEFLAGS   =
else
Q           = @
PRINT       = @printf
REDIRECT    = >/dev/null
MAKEFLAGS   = --no-print-directory --silent
endif

# Default install paths
prefix     ?= /usr/local
libdir     ?= $(prefix)/lib
datadir    ?= $(prefix)/share/doc/$(LIBNAME)
incdir     ?= $(prefix)/include

# Pretty printing and GCC -M for auto dep files
%.o: %.c
	$(PRINT) "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/)$@\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c -MMD -MP -o $@ $<

%: %.o
	$(PRINT) "  LINK    $(subst $(ROOTDIR)/,,$(shell pwd)/)$@\n"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-Map,$@.map -o $@ $^ $(LDLIBS$(LDLIBS-$(@)))

%.test: %.c
	$(PRINT) "  TEST    $(subst $(ROOTDIR)/,,$(shell pwd)/)$(@:.test=)\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -g -o $@ -DUNITTEST $< $(OBJS)
	$(Q)@./$@ $(REDIRECT)


# Default build rules for both main and unit test makefiles
all:: $(TARGET)

clean::
	-$(Q)$(RM) $(OBJS) $(DEPS) $(TARGET) $(SOLIB) $(STATICLIB)

distclean::
	-$(Q)$(RM) $(JUNK) unittest *.o *.a *.so* *.unittest *.test

