enum { Charging, Discharging, NotCharging, Full, Unknown };
enum { Unblock, BlockSw, BlockHw };
enum { Mute, Unmute };

struct ps_info {
	int perc;
	int status;
};

struct mixer {
	char *card;
	char *name;
	int   index;
};

int battery_getinfo(struct ps_info *info, const char *battery);

int rfkill_getinfo(int *info, const char *rfdev);

int volume_alsa_getinfo(struct ps_info *info, const struct mixer *mixer);

int volume_pulse_getinfo(struct ps_info *info, const char *sink);
