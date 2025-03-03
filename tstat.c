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

struct rfkill_state_map {
	int   state;
	char *fmt;
};

static const char *_ps_stat(struct ps_info *info, const struct ps_map *map,
                            size_t len);
const char *battery(const void *battery);
const char *datetime(const void *fmt);
const char *rfkill(const void *name);
const char *volume_alsa(const void *mixer);
const char *volume_pulse(const void *sink);

static char buf[BFR_MAX];

#include "config.h"

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

const char *battery(const void *battery)
{
	struct ps_info info;

	if (battery_getinfo(&info, (char *) battery) != 0)
		return NULL;

	return _ps_stat(&info, battery_map, LEN(battery_map));
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

const char *rfkill(const void *name)
{
	struct rfkill_info info;
	const struct rfkill_state_map *map;
	int state;
	size_t len, i;

	if (rfkill_getinfo(&info, (char *) name) != 0)
		return NULL;

	if (info.hard_state)
		state = RFKILL_STATE_HARD_BLOCKED;
	else if (info.soft_state)
		state = RFKILL_STATE_SOFT_BLOCKED;
	else
		state = RFKILL_STATE_UNBLOCKED;

	if (info.type == RFKILL_TYPE_WLAN) {
		map = wifi_map;
		len = LEN(wifi_map);
	} else {
		map = bluetooth_map;
		len = LEN(bluetooth_map);
	}

	for (i = 0; i < len; i++) {
		if (state == map[i].state)
			return map[i].fmt;
	}

	return NULL;
}

const char *volume_alsa(const void *mixer)
{
	struct ps_info info;

	if (volume_alsa_getinfo(&info, (struct mixer *) mixer) != 0)
		return NULL;

	return _ps_stat(&info, volume_map, LEN(volume_map));
}

const char *volume_pulse(const void *sink)
{
	struct ps_info info;

	if (volume_pulse_getinfo(&info, (char *) sink) != 0)
		return NULL;

	return _ps_stat(&info, volume_map, LEN(volume_map));
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
