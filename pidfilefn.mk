CFLAGS      = -W -Wall -Wextra -Werror
OBJS        = pidfile.o chomp.o fexist.o
ASROOT      = 1
TARGET      = clean $(OBJS) pidfilefn.test

include rules.mk
