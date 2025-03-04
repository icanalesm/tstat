#include <string.h>

#include "info.h"
#include "../util.h"

#define POWER_SUPPLY_ATTRIBUTE_FMT    "/sys/class/power_supply/%s/%s"
#define BFR_MAX                       128

int battery_getinfo(struct ps_info *info, const char *battery)
{
        int perc;
        size_t i;
        char bfr[BFR_MAX];
        char *state[] = {
                [POWER_SUPPLY_STATUS_UNKNOWN]      = "Unknown",
                [POWER_SUPPLY_STATUS_CHARGING]     = "Charging",
                [POWER_SUPPLY_STATUS_DISCHARGING]  = "Discharging",
                [POWER_SUPPLY_STATUS_NOT_CHARGING] = "Not charging",
                [POWER_SUPPLY_STATUS_FULL]         = "Full",
        };

        if (esnprintf(bfr, sizeof(bfr), POWER_SUPPLY_ATTRIBUTE_FMT, battery,
                      "capacity") < 0)
                return -1;
        if (epscanf(bfr, "%d", &perc) != 1)
                return -1;

        if (esnprintf(bfr, sizeof(bfr), POWER_SUPPLY_ATTRIBUTE_FMT, battery,
                      "status") < 0)
                return -1;
        if (epscanf(bfr, "%12s", bfr) != 1)
                return -1;

        for (i = 0; i < LEN(state); i++) {
                if (strcmp(state[i], bfr) == 0)
                        break;
        }

        info->perc = perc;
        info->status = (i == LEN(state)) ? -1 : (int) i;

        return 0;
}
