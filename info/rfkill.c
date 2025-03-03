#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <linux/rfkill.h>

#include "info.h"
#include "../util.h"

#define RFKILL_DEV_PATH         "/dev/rfkill"
#define RFKILL_ATTRIBUTE_FMT    "/sys/class/rfkill/rfkill%u/%s"
#define BFR_MAX                  128

static int rfkill_open()
{
        int fd;

        fd = open(RFKILL_DEV_PATH, O_RDONLY);
        if (fd < 0) {
                error("cannot open %s", RFKILL_DEV_PATH);
                return -errno;
        }

        if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
                error("cannot set non-blocking %s", RFKILL_DEV_PATH);
                close(fd);
                return -errno;
        }

        return fd;
}

/* returns: 0 success, 1 read again, < 0 error */
static int rfkill_read_event(int fd, struct rfkill_event *event)
{
        ssize_t	len = read(fd, event, sizeof(*event));

        if (len < 0) {
                if (errno == EAGAIN)
                        return 1;
                error("cannot read %s", RFKILL_DEV_PATH);
                return -errno;
        }

        if ((size_t) len < (size_t) RFKILL_EVENT_SIZE_V1) {
                error("wrong size of rfkill event: %zu < %zu", (size_t) len,
                      (size_t) RFKILL_EVENT_SIZE_V1);
                return 1;
        }

        return 0;
}

static const char *rfkill_get_attr(uint32_t idx, const char *attr)
{
        static char bfr[BFR_MAX];

        if (esnprintf(bfr, sizeof(bfr), RFKILL_ATTRIBUTE_FMT, idx, attr) < 0)
                return NULL;
        if (epscanf(bfr, "%s", bfr) != 1)
                return NULL;

        return bfr;
}

static int event_match(struct rfkill_event *event, const char *name)
{
        if (event->op != RFKILL_OP_ADD)
                return 0;
        if (event->type != RFKILL_TYPE_WLAN &&
            event->type != RFKILL_TYPE_BLUETOOTH)
                return 0;
        if (strcmp(name, rfkill_get_attr(event->idx, "name")) != 0)
                return 0;
        return 1;
}

int rfkill_getinfo(struct rfkill_info *info, const char *name)
{
        struct rfkill_event event;
        int fd, rc;

        fd = rfkill_open();
        if (fd < 0)
                return fd;

        while (1) {
                rc = rfkill_read_event(fd, &event);
                if (rc < 0)
                        break;
                if (rc == 1 && errno == EAGAIN)
                        break; /* done */
                if (rc == 0 && event_match(&event, name)) {
                        info->type = event.type;
                        info->soft_state = event.soft;
                        info->hard_state = event.hard;
                        rc = 0;
                        break;
                }
        }
        close(fd);

        return rc;
}
