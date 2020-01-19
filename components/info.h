//enum battery_status { Charging, Discharging, NotCharging, Full, Unknown };
//enum rfkill_status { Unblock, BlockSw, BlockHw }
//enum volume_status { Mute, Unmute };
enum { Charging, Discharging, NotCharging, Full, Unknown };
enum { Unblock, BlockSw, BlockHw };
enum { Mute, Unmute };

struct battery_info {
	int perc;
	int status;
};

struct volume_info {
	int perc;
	int status;
};

struct mixer {
	char *card;
	char *name;
	int   index;
};

int battery_getinfo(struct battery_info *info, const char *battery);

int rfkill_getinfo(int *info, const char *rfdev);

int volume_getinfo(struct volume_info *info, const struct mixer *mixer);