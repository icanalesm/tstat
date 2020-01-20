#include "info.h"
#include "../util.h"

#define RFKL_STATUSH_FMT    "/sys/class/rfkill/%s/hard"
#define RFKL_STATUSS_FMT    "/sys/class/rfkill/%s/soft"
#define BFR_MAX             128

int rfkill_getinfo(int *info, const char *rfdev)
{
	int block;
	char bfr[BFR_MAX];

	if (esnprintf(bfr, sizeof(bfr), RFKL_STATUSS_FMT, rfdev) < 0)
		return -1;
	if (epscanf(bfr, "%d", &block) != 1)
		return -1;

	if (block) {
		*info = BlockSw;
		return 0;
	}

	if (esnprintf(bfr, sizeof(bfr), RFKL_STATUSH_FMT, rfdev) < 0)
		return -1;
	if (epscanf(bfr, "%d", &block) != 1)
		return -1;

	*info = (block) ? BlockHw : Unblock;

	return 0;
}
