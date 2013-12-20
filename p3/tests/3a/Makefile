
INSTALL_PATH=~cs537-2/ta/tests/3a

all: runtests libmem.so random tests

runtests:
	echo "#!/bin/bash" > $@
	echo "base=$(INSTALL_PATH)" >> $@
	echo "python \$$base/project3a.py --test-path \$$base \$$@"\
		" | tee runtests.log" >> $@
	echo "exit \$$?" >> $@
	chmod a+x runtests

install: runtests
	mkdir -p $(INSTALL_PATH)
	cp -r * $(INSTALL_PATH)
	afs_rseta $(INSTALL_PATH) system:anyuser read

check:
	pychecker --only *.py

clean:
	rm -f runtests
	rm -f *.pyc
	make -C tests clean
	rm -f libmem.so skeleton.o random

.PHONY: tests
tests:
	make -C tests

libmem.so: skeleton.o
	gcc -g -shared skeleton.o -o libmem.so

skeleton.o: skeleton.c
	gcc -g -c -fpic -o skeleton.o skeleton.c

random: random.c libmem.so mem.h
	gcc -g -L. -lmem -Xlinker -rpath=. -o $@ $<


