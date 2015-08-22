.PHONY: all clean distclean

# Figure out root of library, unless used as submodule
ROOTDIR    ?= $(shell pwd)

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

export ROOTDIR Q PRINT MAKEFLAGS

# Pretty printing and GCC -M for auto dep files
%.o: %.c
	$(PRINT) "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/)$@\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c -MMD -MP -o $@ $<

%: %.o
	$(PRINT) "  LINK    $(subst $(ROOTDIR)/,,$(shell pwd)/)$@\n"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-Map,$@.map -o $@ $^ $(LDLIBS$(LDLIBS-$(@)))

%.test: %.c
	$(PRINT) "  TEST    $(subst $(ROOTDIR)/,,$(shell pwd)/)$(@:.test=)\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -DUNITTEST $< $(OBJS)
	$(Q)@./$@ $(REDIRECT)


# Default build rules for both main and unit test makefiles
all:: $(TARGET)

clean::
	-$(Q)$(RM) $(OBJS) $(DEPS) $(TARGET) $(SOLIB) $(STATICLIB)

distclean::
	-$(Q)$(RM) $(JUNK) unittest *.o *.a *.so* *.unittest *.test

