OBJS        = dir.o copyfile.o fexist.o fisdir.o fmode.o
TARGET      = $(OBJS) rsync.test clean

include rules.mk
