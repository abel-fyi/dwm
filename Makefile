# dwm and local status bar

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
	rm -f dwm dwm-status ${OBJ}

.PHONY: all clean
