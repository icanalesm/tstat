#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "info/info.h"
#include "util.h"

struct arg {
	const char *(*func)();
	const char *fmt;
	const void *arg;
};

struct map_ps {
	int   perc;
	int   status;
	char *fmt;
};

const char *battery(const void *battery);
const char *bluetooth(const void *rfdev);
const char *datetime(const void *fmt);
const char *volume(const void *mixer);
const char *wifi(const void *rfdev);

static char buf[64];

#include "config.h"

const char *battery(const void *battery)
{
	struct battery_info info;
	size_t i;

	if (battery_getinfo(&info, (char *) battery) != 0)
		return NULL;

	for (i = 0; i < LEN(battery_map); i++) {
		if (info.perc <= battery_map[i].perc &&
		    info.status == battery_map[i].status) {
			esnprintf(buf, sizeof(buf), battery_map[i].fmt, info.perc);
			return buf;
		}
	}

	return NULL;
}

const char *bluetooth(const void *rfdev)
{
	int info;
	size_t i;

	if (rfkill_getinfo(&info, (char *) rfdev) != 0)
		return NULL;
	for (i = 0; i < LEN(btooth_map); i++) {
		if (info == btooth_map[i].status)
			return btooth_map[i].fmt;
	}

	return NULL;
}

const char *datetime(const void *fmt)
{
	time_t t;

	t = time(NULL);
	if (!strftime(buf, sizeof(buf), (char *) fmt, localtime(&t))) {
		error("datetime: Result string exceeds buffer size");
		return NULL;
	}

	return buf;
}

const char *volume(const void *mixer)
{
	struct volume_info info;
	size_t i;

	if (volume_getinfo(&info, (struct mixer *) mixer) != 0)
		return NULL;

	for (i = 0; i < LEN(volume_map); i++) {
		if (info.perc <= volume_map[i].perc &&
		    info.status == volume_map[i].status) {
			esnprintf(buf, sizeof(buf), volume_map[i].fmt, info.perc);
			return buf;
		}
	}

	return NULL;
}

const char *wifi(const void *rfdev)
{
	int info;
	size_t i;

	if (rfkill_getinfo(&info, (char *) rfdev) != 0)
		return NULL;

	for (i = 0; i < LEN(wifi_map); i++) {
		if (info == wifi_map[i].status)
			return wifi_map[i].fmt;
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	Display *dpy;
	char status[MAXLEN];
	const char *res;
	size_t len, i;
	int ret;

	/* Get status string */
	status[0] = '\0';
	for (i = len = 0; i < LEN(args); i++) {
		if (!(res = args[i].func(args[i].arg)))
			res = unknown_str;
		if ((ret = esnprintf(status + len, sizeof(status) - len,
                                     args[i].fmt, res)) < 0)
			break;
		len += ret;
	}

	/* Show status string */
	if (argc > 1 && strcmp(argv[1], "set") == 0) {
		if (!(dpy = XOpenDisplay(NULL))) {
			error("XOpenDisplay: Failed to open display");
			exit(1);
		}
		if (XStoreName(dpy, DefaultRootWindow(dpy), status) < 0)
			error("XStoreName: Allocation failed");
		else
			XFlush(dpy);
		if (XCloseDisplay(dpy) < 0) {
			error("XCloseDisplay: Failed to close display");
			exit(1);
		}
	} else {
		puts(status);
		fflush(stdout);
		if (ferror(stdout)) {
			error("puts: %s", strerror(errno));
			exit(1);
		}
	}

	return 0;
}
