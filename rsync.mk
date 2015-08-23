OBJS        = dir.o copyfile.o fexist.o fisdir.o fmode.o tree.o
TARGET      = clean $(OBJS) rsync.test

include rules.mk
