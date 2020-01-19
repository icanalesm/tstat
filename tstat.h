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

/* Battery */
const char *battery(const void *battery);

/* Bluetooth */
const char *bluetooth(const void *rfdev);

/* Date */
const char *datetime(const void *fmt);

/* Volume */
const char *volume(const void *mixer);

/* Wi-Fi */
const char *wifi(const void *rfdev);
