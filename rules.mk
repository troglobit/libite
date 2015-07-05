# Pretty printing and GCC -M for auto dep files
%.o: %.c
	$(PRINT) "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/)$@\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c -MMD -MP -o $@ $<

%: %.o
	$(PRINT) "  LINK    $(subst $(ROOTDIR)/,,$(shell pwd)/)$@\n"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-Map,$@.map -o $@ $^ $(LDLIBS$(LDLIBS-$(@)))

