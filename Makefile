NAME=sxpdf
VERSION=0.0.1
# Linux、Haiku、かIllumos = /usr、FreeBSDかOpenBSD = /usr/local、NetBSD = /usr/pkg
PREFIX=/usr
CC=gcc
CFLAGS=-Wall $(shell pkg-config --cflags poppler-glib)
LDFLAGS=-lX11 -lcairo $(shell pkg-config --libs poppler-glib)

all:
	${CC} ${CFLAGS} -o ${NAME} main.c ${LDFLAGS}

clean:
	rm -f ${NAME}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean install uninstall
