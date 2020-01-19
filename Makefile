include config.mk

COM =\
	components/battery\
	components/rfkill\
	components/volume\
	util\
	tstat

all: options tstat

options:
	@echo tstat build options:
	@echo "CFLAGS = ${CFLAGS}"
	@echo "CC     = ${CC}"

.c.o:
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<

tstat: tstat.o ${COM:=.o}
	${CC} ${LDLIBS} ${COM:=.o} -o $@

clean:
	rm -f tstat tstat.o ${COM:=.o}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f tstat ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/tstat

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/tstat

.PHONY: all options clean install uninstall
