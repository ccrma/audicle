current: 
	@echo "[audicle build]: please use one of the following configurations:"; echo "   make linux-alsa, make linux-jack, make linux-oss"; echo "   make osx, make osx-ub, or make win32"

install:
	@echo "(must be root to install)"
	cp $(wildcard audicle audicle.exe) /usr/bin/; chmod 755 /usr/bin/$(wildcard audicle audicle.exe)

osx: 
	-make -f makefile.osx

osx-ub:
	-make -f makefile.osx-ub

linux-oss: 
	-make -f makefile.oss 

linux-jack:
	-make -f makefile.jack

linux-alsa: 
	-make -f makefile.alsa

win32: 
	-make -f makefile.win32

clean:
	rm -f *.o chuck.tab.h chuck.tab.c chuck.yy.c chuck.output $(wildcard audicle audicle.exe)
