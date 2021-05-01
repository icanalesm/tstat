#include <string.h>

#include "info.h"
#include "../util.h"

#define BAT_CAPACITY_FMT    "/sys/class/power_supply/%s/capacity"
#define BAT_STATUS_FMT      "/sys/class/power_supply/%s/status"
#define BAT_STATUS_NUM      5
#define BFR_MAX             128

int battery_getinfo(struct ps_info *info, const char *battery)
{
	int perc;
	size_t i;
	char bfr[BFR_MAX];
	char *stat[BAT_STATUS_NUM] = { "Charging", "Discharging",
	                               "Not charging", "Full", "Unknown" };

	if (esnprintf(bfr, sizeof(bfr), BAT_CAPACITY_FMT, battery) < 0)
		return -1;
	if (epscanf(bfr, "%d", &perc) != 1)
		return -1;

	if (esnprintf(bfr, sizeof(bfr), BAT_STATUS_FMT, battery) < 0)
		return -1;
	if (epscanf(bfr, "%12s", bfr) != 1)
		return -1;

	for (i = 0; i < BAT_STATUS_NUM; i++) {
		if (!strcmp(stat[i], bfr))
			break;
	}

	info->perc = perc;
	info->status = (i == BAT_STATUS_NUM) ? -1 : (int) i;

	return 0;
}
