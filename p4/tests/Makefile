
INSTALL_PATH=~cs537-2/ta/tests/4b

runtests:
	echo "#!/bin/bash" > $@
	echo "base=$(INSTALL_PATH)" >> $@
	echo "python \$$base/project4b.py --test-path \$$base \$$@ |"\
		"tee -i runtests.log" >> $@
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
