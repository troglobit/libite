CFLAGS     += -g -O0
CPPFLAGS   += -D_GNU_SOURCE

ifeq ($V,1)
VERBOSE     = -v
REDIRECT    =
else
VERBOSE     =
REDIRECT    = >/dev/null
endif

# Some tests may need to be run as root, e.g. pidfile()
ifeq ($(ASROOT), 1)
SUDO        = sudo -n
else
SUDO        =
endif

all: $(TARGET)

%.test: %.c
	@printf "  TEST    $@\n"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -g -o $@ -DUNITTEST $< $(OBJS)
	@$(RM) $(OBJS)
	@$(SUDO) ./$@ $(VERBOSE) $(REDIRECT)

clean::
	-@$(RM) $(OBJS) $(DEPS) $(TARGET) $(SOLIB) $(STATICLIB)
	-@$(RM) $(JUNK) unittest *.o *.a *.so* *.unittest *.test

