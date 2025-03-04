#include <linux/rfkill.h>

enum { Mute, Unmute };

/**
 * Possible states of a battery
 */
enum battery_state {
        POWER_SUPPLY_STATUS_UNKNOWN = 0,
        POWER_SUPPLY_STATUS_CHARGING,
        POWER_SUPPLY_STATUS_DISCHARGING,
        POWER_SUPPLY_STATUS_NOT_CHARGING,
        POWER_SUPPLY_STATUS_FULL,
};

/**
 * Definition of an ALSA device.
 */
struct mixer {
        char *card;  /**< ALSA card. */
        char *name;  /**< ALSA name. */
        int   index; /**< ALSA index. */
};

/**
 * Percentage and state information.
 */
struct ps_info {
        int perc;   /**< Percentage. */
        int status; /**< State of the device. */
};

/**
 * Information of an rfkill device.
 */
struct rfkill_info {
        enum rfkill_type type;       /**< Type of rfkill device. */
        int              soft_state; /**< Softblock state. */
        int              hard_state; /**< Hardblock state. */
};

/**
 * Obtain information of a battery.
 *
 * Obtain the capacity in percentage and status of a battery.
 *
 * @param info Pointer to store the information in.
 * @param battery Name of the battery.
 * @return 0 on success or -1 if an error occurred.
 */
int battery_getinfo(struct ps_info *info, const char *battery);

/**
 * Obtain information of an rfkill device.
 *
 * Obtain the type, softblock state and hardblock state of an rfkill device.
 * Currently, supported types are RFKILL_TYPE_WLAN and RFKILL_TYPE_BLUETOOTH
 * only. The possible values for softblock and hardblock states are 0
 * (unblocked) and 1 (blocked).
 *
 * @param info Pointer to store the information in.
 * @param name Name of the rfkill device.
 */
int rfkill_getinfo(struct rfkill_info *info, const char *name);

/**
 * Obtain volume information of an ALSA device.
 *
 * @param info Pointer to store the information in.
 * @param mixer Pointer to the definition of the ALSA device.
 *
 * Obtain the volume in percentage and status of an ALSA device. The possible
 * status are "Mute" and "Unmute".
 */
int volume_alsa_getinfo(struct ps_info *info, const struct mixer *mixer);

/**
 * Obtain volume information of a PulseAudio sink.
 *
 * Obtain the volume in percentage and status of a PulseAudio sink. The possible
 * status are "Mute" and "Unmute".
 *
 * @param info Pointer to store the information in.
 * @param sink Name of the PulseAudio sink.
 */
int volume_pulse_getinfo(struct ps_info *info, const char *sink);
