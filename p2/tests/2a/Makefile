
INSTALL_PATH:=~cs537-2/ta/tests/2a
PYTHON_PATH=python

all: runtests

runtests:
	echo "#!/bin/bash" > $@
	echo "base=$(INSTALL_PATH)" >> $@
	echo "$(PYTHON_PATH) \$$base/project2a.py --test-path \$$base \$$@"\
		" | tee runtests.log" >> $@
	echo "exit \$$?" >> $@
	chmod a+x runtests

install: runtests
	mkdir -p $(INSTALL_PATH)
	cp -r * $(INSTALL_PATH)
	afs_rseta $(INSTALL_PATH) system:anyuser read

clean:
	rm -f runtests
	rm -f *.pyc
