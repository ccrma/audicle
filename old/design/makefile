#makefile switch


osx :
	make -f makefile.osx
ckx-osx : 
	make -f makefile.osx ckx

linux : 
	make -f makefile.linux

ckx-linux :
	make -f makefile.linux ckx

clean : 
	rm *.o texteditor

install : osx
	sudo cp texteditor /usr/bin/audicle
