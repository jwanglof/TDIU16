BUILD_SUBDIRS = threads userprog vm filesys

all::
	@echo "Run 'make' in subdirectories: $(BUILD_SUBDIRS)."
	@echo "This top-level make has only 'clean' targets."

CLEAN_SUBDIRS = $(BUILD_SUBDIRS) examples utils

clean::
	for d in $(CLEAN_SUBDIRS); do $(MAKE) -C $$d $@; done
	rm -f TAGS tags

distclean:: clean
	find . -name '*~' -exec rm '{}' \;
	find . -name 'fs.dsk' -exec rm '{}' \;

TAGS_SUBDIRS = $(BUILD_SUBDIRS) devices lib
TAGS_SOURCES = find $(TAGS_SUBDIRS) -name \*.[chS] -print

TAGS::
	etags --members `$(TAGS_SOURCES)`

tags::
	ctags -T --no-warn `$(TAGS_SOURCES)`

cscope.files::
	$(TAGS_SOURCES) > cscope.files

cscope:: cscope.files
	cscope -b -q -k

SKEL_SRC = $(shell find . -name '*.[ch]' -exec /usr/xpg4/bin/egrep -q '//--*\[ .*\]--*//' {} \; -print)

skel:: TAGS
	@for i in $(SKEL_SRC); do			\
		echo "Processing $$i";			\
		mv $$i $$i~~;				\
		/home/klaar/bin/strip $$i~~ $$i;	\
	done

REVERT = $(subst ~~,,$(shell find . -name '*~~'))

revert::
	@for i in $(REVERT); do			\
		echo "Processing $$i";			\
		mv $$i~~ $$i;				\
	done
