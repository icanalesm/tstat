include config.mk

COM =\
	util\
	tstat

all: options tstat

options:
	@echo tstat build options:
	@echo "CFLAGS = ${CFLAGS}"
	@echo "CC     = ${CC}"

util.o: util.c util.h
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

tstat.o: tstat.c tstat.h config.h util.h
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

tstat: ${COM:=.o}
	${CC} -o $@ ${COM:=.o} ${LDLIBS}

clean:
	rm -f tstat tstat.o ${COM:=.o}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f tstat tstatr ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/tstat
	chmod 755 ${DESTDIR}${PREFIX}/bin/tstatr

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/tstat\
	      ${DESTDIR}${PREFIX}/bin/tstatr

.PHONY: all options clean install uninstall
