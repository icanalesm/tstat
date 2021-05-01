include config.mk

REQ =\
	info/battery\
	info/rfkill\
	info/volume\
	info/volume_pulse\
	util\
	tstat

all: options tstat

options:
	@echo tstat build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CPPFLAGS = ${CPPFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "LDLIBS   = ${LDLIBS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

tstat: ${REQ:=.o}
	${CC} ${CFLAGS} ${LDFLAGS} ${LDLIBS} ${REQ:=.o} -o $@

clean:
	rm -f tstat ${REQ:=.o}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f tstat ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/tstat

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/tstat

.PHONY: all options clean install uninstall
