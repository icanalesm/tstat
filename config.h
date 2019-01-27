/* Maximum length of status string */
#define MAXLEN 256

/* Text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* Battery state map */
/* Possible state values: 
 *     "Unknown", "Charging", "Discharging", "Not charging", "Full"
 */
static const struct map_ps batst[] = {
	/* perc  state           str */
	{ 100,   "Charging",     "\uf0e7"    },
	{  10,   "Discharging",  "\uf244"    },
	{  35,   "Discharging",  "\uf243"    },
	{  60,   "Discharging",  "\uf242"    },
	{  80,   "Discharging",  "\uf241"    },
	{ 100,   "Discharging",  "\uf240",   },
	{ 100,   "Full",         "\uf1e6",   },
	{ 100,   "Unknown",      "\uf244(?)" },
};

/* Bluetooth state map */
/* Possible state values: 
 *     "On", "Off"
 */
static const struct map_ps btst[] = {
	/* perc  state   str */
	{ -1,    "On",   "on" },
	{ -1,    "Off",  "off" },
};

/* Volume state map */
/* Possible state values: 
 *     "Unmute", "Mute"
 */
static const struct map_ps volst[] = {
	/* perc  state      str */
	{   0,   "Unmute",  "\uf026  " },
	{  30,   "Unmute",  "\uf027 " },
	{ 100,   "Unmute",  "\uf028" },
	{ 100,   "Mute",    "\uf6a9" },
};

/* Wi-Fi state map */
/* Possible state values: 
 *     "On", "Off"
 */
static const struct map_ps wifist[] = {
	/* perc  state   str */
	{ -1,    "On",   "on" },
	{ -1,    "Off",  "off" },
};

/* Mixer device definition */
static const struct mixer mixers[] = {
	/* card       name       index */
	{ "default",  "Master",  0 },
};

/*
 * function            description                      argument                   example
 *
 * battery             battery status and percentage    battery name string        "BAT0"
 * bluetooth_state     bluetooth state                  rfkill name string         "rfkill1"
 * date time           date and time                    format string              "%F %T"
 * volume              volume status and percentage     pointer to mixer struct    &mixers[0]
 * wifi_state          wifi state                       rfkill name string         "rfkill0"
 */
static const struct arg args[] = {
	/* function         fmt              argument */
	{ bluetooth_state,  "\uf294 %s   ",  "rfkill1" },
	{ wifi_state,       "\uf1eb %s   ",  "rfkill0" },
	{ volume,           "%s   ",         &mixers[0] },
	{ battery,          "%s   ",         "BAT0" },
	{ datetime,         "%s",            "%b %d, %R" },
};

