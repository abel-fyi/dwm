# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = drw.c dwm.c util.c
OBJ = ${SRC:.c=.o}

all: dwm dwm-status

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

dwm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

dwm-status: dwm-status.c config.mk
	${CC} ${CFLAGS} -o $@ dwm-status.c -lX11 -lpulse

clean:
	rm -f dwm dwm-status ${OBJ} dwm-${VERSION}.tar.gz

dist: clean
	mkdir -p dwm-${VERSION}
	cp -R LICENSE Makefile README.md config.h config.def.h config.mk\
		dwm.1 drw.h util.h ${SRC} dwm-status.c\
		gaps-6.8.diff pertag-6.8.diff swaptags-6.8.diff alpha-6.8.diff dwm-${VERSION}
	tar -cf dwm-${VERSION}.tar dwm-${VERSION}
	gzip dwm-${VERSION}.tar
	rm -rf dwm-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm
	cp -f dwm-status ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm-status
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < dwm.1 > ${DESTDIR}${MANPREFIX}/man1/dwm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/dwm.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/dwm\
		${DESTDIR}${PREFIX}/bin/dwm-status\
		${DESTDIR}${MANPREFIX}/man1/dwm.1

.PHONY: all clean dist install uninstall
