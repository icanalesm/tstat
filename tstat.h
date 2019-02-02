struct arg {
	const char *(*func)();
	const char *fmt;
	const void *arg;
};

struct map_ps {
	int   perc;
	char *state;
	char *fmt;
};

struct mixer {
	char *card;
	char *name;
	int   index;
};


/* Battery */
const char *battery(const void *bat);

/* Bluetooth */
const char *bluetooth_state(const void *rfdev);

/* Date */
const char *datetime(const void *fmt);

/* Volume */
const char *volume(const void *mixer);

/* Wi-Fi */
const char *wifi_state(const void *rfdev);

