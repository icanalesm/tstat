#include <linux/rfkill.h>

enum { Charging, Discharging, NotCharging, Full, Unknown };
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

struct rfkill_info {
	enum rfkill_type type;
	int              soft_state;
	int              hard_state;
};

int battery_getinfo(struct ps_info *info, const char *battery);

int rfkill_getinfo(struct rfkill_info *info, const char *name);

int volume_alsa_getinfo(struct ps_info *info, const struct mixer *mixer);

int volume_pulse_getinfo(struct ps_info *info, const char *sink);
