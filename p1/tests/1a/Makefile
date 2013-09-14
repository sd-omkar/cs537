
INSTALL_PATH:=~cs537-2/ta/tests/1a
PYTHON_PATH=python

all: runtests dupdata generate

runtests:
	echo "#!/bin/bash" > $@
	echo "base=$(INSTALL_PATH)" >> $@
	echo "$(PYTHON_PATH) \$$base/runtests.py --test-path \$$base \$$@"\
		" | tee runtests.log" >> $@
	echo "exit \$$?" >> $@
	chmod a+x runtests

install: runtests dupdata generate
	mkdir -p $(INSTALL_PATH)
	cp -r * $(INSTALL_PATH)
	afs_rseta $(INSTALL_PATH) system:anyuser read

check:
	pychecker --only *.py

clean:
	rm -f runtests
	rm -f *.pyc
	rm -f dupdata generate


dupdata: generate
	./generate -o $@.tmp -s 0 -n 3
	dd if=/dev/zero bs=100 count=2 | cat - $@.tmp > $@
	rm -f $@.tmp

generate: generate.c sort.h
	gcc -O -Wall -o $@ $<
