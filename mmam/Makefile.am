SUBDIRS = src

EXTRA_DIST = data/*.* data/custom/* doc/html/*.html doc/html/pict/*.jpg


install-data-hook:
	$(INSTALL) -m 755 -d  $(pkgdatadir)
	$(INSTALL) -m 644  data/*.dat data/*.it data/*.ogg $(pkgdatadir) || true
	dat $(pkgdatadir)/levels.dat -c1 -a -t DATA data/custom/*.mmam
	dat $(pkgdatadir)/baseconf.dat -c1 -a -t DATA data/custom/*.conf

src-only-dist:
	rm -rf mmam-src-only-${VERSION} || true
	mkdir mmam-src-only-${VERSION}
	mkdir mmam-src-only-${VERSION}/src
	cp aclocal.m4  install-sh AUTHORS COPYING Makefile.am Makefile.in NEWS README ChangeLog config.sub depcomp compile    configure missing  configure.in INSTALL mkinstalldirs mmam-src-only-${VERSION}
	cp src/*.cc src/*.h src/fonts.dat src/*.pl src/*.sh src/Makefile.am src/Makefile.in src/config.h.in src/stamp-h.in mmam-src-only-${VERSION}/src
	tar -cvzf mmam-src-only-${VERSION}.tar.gz mmam-src-only-${VERSION} 
	rm -rf mmam-src-only-${VERSION}

binary-win:
	mkdir win-binary
	mkdir win-binary/doc
	mkdir win-binary/sav
	upx -9v src/mmam.exe -o win-binary/mmam.exe || cp  src/mmam.exe win-binary/mmam.exe
	dat win-binary/levels.dat -c1 -a -t DATA 'data/custom/*.mmam' || true
	dat win-binary/baseconf.dat -c1 -a -t DATA 'data/custom/*.conf' || true
	cp data/*.dat data/*.it data/*.ogg win-binary || true
	cp c:/windows/system/alleg41.dll win-binary
	cp c:/windows/system/adim20.dll win-binary
	cp -f COPYING win-binary/copying.txt
	cp -f NEWS win-binary/news.txt
	cp -f README win-binary/readme.txt
	cp -f ChangeLog win-binary/changelog.txt
	cp -Rf doc/html/* win-binary/doc
	cp data/win-mmam.cfg win-binary/mmam.cfg || true
	cd win-binary; zip -r9 ../mmam-win-${VERSION}.zip * || tar -czf ../mmam-win-${VERSION}.tar.gz *;cd ..;
	makensis /DVERSION=${VERSION} data/mmam.nsi
	rm -Rf win-binary

strip-data:
	rm data/sprites.dat data/sounds.dat data/*.it data/*.ogg || true
