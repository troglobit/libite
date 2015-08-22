OBJS        = pidfile.o chomp.o
TARGET      = $(OBJS) pidfilefn.test clean
CPPFLAGS    = -D_PATHS_H_ -D_PATH_VARRUN="\"/tmp/\""

include rules.mk
