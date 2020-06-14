include ../unix/env.mk

SUBDIR=beai3

all:
	set -e; \
	if [ -n "$(SUBDIR)" ]; then \
		for i in $(SUBDIR); \
		do \
			cd $$i; \
			make -f Makefile; \
			cd -; \
		done; \
	fi

clean:
	set -e; \
	if [ -n "$(SUBDIR)" ]; then \
		for i in $(SUBDIR); \
		do \
			cd $$i; \
			make -f Makefile clean; \
			cd -; \
		done; \
	fi
	rm -rf .lib

install:all
	set -e; \
	if [ -n "$(SUBDIR)" ]; then \
		for i in $(SUBDIR); \
		do \
			cd $$i; \
			make -f Makefile install; \
			cd -; \
		done; \
	fi

depend:
	set -e; \
	if [ -n "$(SUBDIR)" ]; then \
		for i in $(SUBDIR); \
		do \
			cd $$i; \
			make -f Makefile depend; \
			cd -; \
		done; \
	fi

depend_clean:
	set -e; \
	if [ -n "$(SUBDIR)" ]; then \
		for i in $(SUBDIR); \
		do \
			cd $$i; \
			make -f Makefile depend_clean; \
			cd -; \
		done; \
	fi

realclean:
	set -e; \
	if [ -n "$(SUBDIR)" ]; then \
		for i in $(SUBDIR); \
		do \
			cd $$i; \
			make -f Makefile realclean; \
			cd -; \
		done; \
	fi

uninstall:
	set -e; \
	if [ -n "$(SUBDIR)" ]; then \
		for i in $(SUBDIR); \
		do \
			cd $$i; \
			make -f Makefile uninstall; \
			cd -; \
		done; \
	fi

