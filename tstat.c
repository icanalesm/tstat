#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "info/info.h"
#include "util.h"

#define BFR_MAX    128

struct arg {
	const char *(*func)();
	const char *fmt;
	const void *arg;
};

struct ps_map {
	int   perc;
	int   status;
	char *fmt;
};

static const char *_rfkill(const void *rfdev, const struct ps_map *map,
                           size_t len);
static const char *_ps_stat(struct ps_info *info, const struct ps_map *map,
                            size_t len);
static const char *battery(const void *battery);
static const char *bluetooth(const void *rfdev);
static const char *datetime(const void *fmt);
static const char *volume_alsa(const void *mixer);
static const char *volume_pulse(const void *sink);
static const char *wifi(const void *rfdev);

static char buf[BFR_MAX];

#include "config.h"

static const char *_rfkill(const void *rfdev, const struct ps_map *map,
                           size_t len)
{
	int info;
	size_t i;

	if (rfkill_getinfo(&info, (char *) rfdev) != 0)
		return NULL;

	for (i = 0; i < len; i++) {
		if (info == map[i].status)
			return map[i].fmt;
	}

	return NULL;
}

static const char *_ps_stat(struct ps_info *info, const struct ps_map *map,
                            size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		if (info->perc <= map[i].perc &&
		    info->status == map[i].status) {
			esnprintf(buf, sizeof(buf), map[i].fmt, info->perc);
			return buf;
		}
	}

	return NULL;
}

static const char *battery(const void *battery)
{
	struct ps_info info;

	if (battery_getinfo(&info, (char *) battery) != 0)
		return NULL;

	return _ps_stat(&info, battery_map, LEN(battery_map));
}

static const char *bluetooth(const void *rfdev)
{
	return _rfkill(rfdev, btooth_map, LEN(btooth_map));
}

static const char *datetime(const void *fmt)
{
	time_t t;

	t = time(NULL);
	if (!strftime(buf, sizeof(buf), (char *) fmt, localtime(&t))) {
		error("datetime: Result string exceeds buffer size");
		return NULL;
	}

	return buf;
}

static const char *volume_alsa(const void *mixer)
{
	struct ps_info info;

	if (volume_alsa_getinfo(&info, (struct mixer *) mixer) != 0)
		return NULL;

	return _ps_stat(&info, volume_map, LEN(volume_map));
}

static const char *volume_pulse(const void *sink)
{
	struct ps_info info;

	if (volume_pulse_getinfo(&info, (char *) sink) != 0)
		return NULL;

	return _ps_stat(&info, volume_map, LEN(volume_map));
}

static const char *wifi(const void *rfdev)
{
	return _rfkill(rfdev, wifi_map, LEN(wifi_map));
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
			return -1;
		}
		if (XStoreName(dpy, DefaultRootWindow(dpy), status) < 0) {
			error("XStoreName: Allocation failed");
			return -1;
		}
		XFlush(dpy);
		if (XCloseDisplay(dpy) < 0) {
			error("XCloseDisplay: Failed to close display");
			return -1;
		}
	} else {
		puts(status);
		fflush(stdout);
		if (ferror(stdout)) {
			error("puts: %s", strerror(errno));
			return -1;
		}
	}

	return 0;
}
