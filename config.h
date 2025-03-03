/* Maximum length of status string */
#define MAXLEN 256

/* Text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/*
 * Battery state map
 * Possible state values: 
 *     "Unknown", "Charging", "Discharging", "Not charging", "Full"
 */
static const struct ps_map battery_map[] = {
	/* perc  state           fmt */
	{ 100,   Charging,       "\uf0e7 %d%%"    },
	{  10,   Discharging,    "\uf244 %d%%"    },
	{  35,   Discharging,    "\uf243 %d%%"    },
	{  60,   Discharging,    "\uf242 %d%%"    },
	{  80,   Discharging,    "\uf241 %d%%"    },
	{ 100,   Discharging,    "\uf240 %d%%"    },
	{ 100,   Full,           "\uf1e6 %d%%"    },
	{ 100,   Unknown,        "\uf244(?) %d%%" } 
};

/*
 * Volume state map
 * Possible state values:
 *     "Unmute", "Mute"
 */
static const struct ps_map volume_map[] = {
	/* perc  state      fmt */
	{   0,   Unmute,    "\uf026   %ld%%" },
	{  30,   Unmute,    "\uf027  %ld%%"  },
	{ 100,   Unmute,    "\uf028 %ld%%"   },
	{ 100,   Mute,      "\uf6a9 %ld%%"   } 
};

/*
 * Bluetooth state map
 * Possible state values:
 *     RFKILL_STATE_UNBLOCKED, RFKILL_STATE_SOFT_BLOCKED,
 *     RFKILL_STATE_HARD_BLOCKED
 */
static const struct rfkill_state_map bluetooth_map[] = {
	/* state                           fmt */
	{ RFKILL_STATE_UNBLOCKED,          "\uf294 on"  },
	{ RFKILL_STATE_SOFT_BLOCKED,       "\uf294 off" },
	{ RFKILL_STATE_HARD_BLOCKED,       "\uf294 off" }
};

/*
 * Wi-Fi state map
 * Possible state values:
 *     RFKILL_STATE_UNBLOCKED, RFKILL_STATE_SOFT_BLOCKED,
 *     RFKILL_STATE_HARD_BLOCKED
 */
static const struct rfkill_state_map wifi_map[] = {
	/* state                           fmt */
	{ RFKILL_STATE_UNBLOCKED,          "\uf1eb on"  },
	{ RFKILL_STATE_SOFT_BLOCKED,       "\uf1eb off" },
	{ RFKILL_STATE_HARD_BLOCKED,       "\uf1eb off" }
};

/* Mixer device definition */
static const struct mixer mixers[] = {
	/* card       name       index */
	{ "default",  "Master",  0 } 
};

/*
 * function            description                      argument                   example
 *
 * battery             battery status and percentage    battery name               "BAT1"
 * datetime            date and time                    format string              "%F %T"
 * rfkill              rfkill device state              rfkill device name         "phy0"
 * volume_alsa         volume status and percentage     pointer to mixer struct    &mixers[0]
 * volume_pulse        volume status and percentage     pulseaudio sink name       "alsa_output.pci-0000_00_1b.0.output_analog-stereo"
 * volume_pulse        volume status and percentage     pulseaudio sink name       "@DEFAULT_SINK@"
 */
static const struct arg args[] = {
	/* function         fmt              argument */
	{ rfkill,           "%s   ",         "hci0" },
	{ rfkill,           "%s   ",         "phy0" },
	{ volume_pulse,     "%s   ",         "@DEFAULT_SINK@" },
	{ battery,          "%s   ",         "BAT0" },
	{ datetime,         "%s",            "%b %d, %R" } 
};
