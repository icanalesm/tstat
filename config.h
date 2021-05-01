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
	{ 100,   Discharging,    "\uf240 %d%%",   },
	{ 100,   Full,           "\uf1e6 %d%%",   },
	{ 100,   Unknown,        "\uf244(?) %d%%" },
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
	{ 100,   Mute,      "\uf6a9 %ld%%"   },
};

/*
 * Bluetooth state map
 * Possible state values: 
 *     "Unblock", "BlockSw", "BlockHw"
 */
static const struct ps_map btooth_map[] = {
	/* perc  state       fmt */
	{ -1,    Unblock,    "\uf294 on"  },
	{ -1,    BlockSw,    "\uf294 off" },
	{ -1,    BlockHw,    "\uf294 off" },
};

/*
 * Wi-Fi state map
 * Possible state values: 
 *     "Unblock", "BlockSw", "BlockHw"
 */
static const struct ps_map wifi_map[] = {
	/* perc  state       fmt */
	{ -1,    Unblock,    "\uf1eb on"  },
	{ -1,    BlockSw,    "\uf1eb off" },
	{ -1,    BlockHw,    "\uf1eb off" },
};

/* Mixer device definition */
static const struct mixer mixers[] = {
	/* card       name       index */
	{ "default",  "Master",  0 },
};

/*
 * function            description                      argument                   example
 *
 * battery             battery status and percentage    battery name               "BAT1"
 * bluetooth           bluetooth state                  rfkill device name         "rfkill1"
 * datetime            date and time                    format string              "%F %T"
 * volume_alsa         volume status and percentage     pointer to mixer struct    &mixers[0]
 * volume_pulse        volume status and percentage     pulseaudio sink name       "alsa_output.pci-0000_00_1b.0.output_analog-stereo"
 * wifi                wifi state                       rfkill device name         "rfkill0"
 */
static const struct arg args[] = {
	/* function         fmt              argument */
	{ bluetooth,        "%s   ",         "rfkill1" },
	{ wifi,             "%s   ",         "rfkill0" },
	{ volume_pulse,     "%s   ",         "alsa_output.pci-0000_00_1b.0.output_analog-stereo" },
	{ battery,          "%s   ",         "BAT0" },
	{ datetime,         "%s",            "%b %d, %R" },
};
