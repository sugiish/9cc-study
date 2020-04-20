subdirs = $(wildcard */.)

.PHONY: all $(subdirs)
all: $(subdirs)
$(subdirs):
	$(MAKE) test -C $@
