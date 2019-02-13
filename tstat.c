#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <X11/Xlib.h>

#include "util.h"
#include "tstat.h"

#define BAT_CAPACITY_FMT  "/sys/class/power_supply/%s/capacity"
#define BAT_STATUS_FMT    "/sys/class/power_supply/%s/status"
#define RFKL_STATUSH_FMT  "/sys/class/rfkill/%s/hard"
#define RFKL_STATUSS_FMT  "/sys/class/rfkill/%s/soft"

static char buf[256];

#include "config.h"

static char *rfkill_blocked(const void *rfdev)
{
	int block;

	if (esnprintf(buf, sizeof(buf), RFKL_STATUSS_FMT, rfdev) < 0)
		return NULL;

	if (epscanf(buf, "%d", &block) != 1)
		return NULL;

	if (block)
		return "Off";

	if (esnprintf(buf, sizeof(buf), RFKL_STATUSH_FMT, rfdev) < 0)
		return NULL;

	if (epscanf(buf, "%d", &block) != 1)
		return NULL;

	return block ? "Off" : "On";
}

const char *battery(const void *bat)
{
	int perc;
	size_t i;

	if (esnprintf(buf, sizeof(buf), BAT_CAPACITY_FMT, (char *)bat) < 0)
		return NULL;

	if (epscanf(buf, "%d", &perc) != 1)
		return NULL;

	if (esnprintf(buf, sizeof(buf), BAT_STATUS_FMT, (char *)bat) < 0)
		return NULL;

	if (epscanf(buf, "%12s", buf) != 1)
		return NULL;

	for (i = 0; i < LEN(batst); i++) {
		if (!strcmp(batst[i].state, buf) && perc <= batst[i].perc) {
			esnprintf(buf, sizeof(buf), batst[i].fmt, perc);
			return buf;
		}
	}

	return NULL;
}

const char *bluetooth_state(const void *rfdev)
{
	char *state;
	size_t i;

	if (!(state = rfkill_blocked(rfdev)))
		return NULL;

	for (i = 0; i < LEN(btst); i++) {
		if (!strcmp(btst[i].state, state))
			return btst[i].fmt;
	}

	return NULL;
}

const char *datetime(const void *fmt)
{
	time_t t;

	t = time(NULL);
	if (!strftime(buf, sizeof(buf), (char *)fmt, localtime(&t))) {
		error("datetime: Result string exceeds buffer size");
		return NULL;
	}

	return buf;
}

const char *volume(const void *mixer)
{
	long vol, min, max;
	int aux;
	size_t i;
	char *s[] = { "Mute", "Unmute" };
	struct mixer *mix = (struct mixer *) mixer;
	snd_mixer_selem_id_t *sid;
	snd_mixer_t *handle;
	snd_mixer_elem_t *elem;

	if ((aux = snd_mixer_selem_id_malloc(&sid)) < 0) {
		error("Mixer allocation error: %s", snd_strerror(aux));
		return NULL;
	}
	snd_mixer_selem_id_set_index(sid, mix->index);
	snd_mixer_selem_id_set_name(sid, mix->name);
	if ((aux = snd_mixer_open(&handle, 0)) < 0) {
		error("Mixer %s open error: %s", mix->card, snd_strerror(aux));
		goto clean_sid;
	}
	if ((aux = snd_mixer_attach(handle, mix->card)) < 0) {
		error("Mixer %s attach error: %s", mix->card, snd_strerror(aux));
		goto clean_handle;
	}
	if ((aux = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
		error("Mixer %s register error: %s", mix->card, snd_strerror(aux));
		goto clean_handle;
	}
 	if ((aux = snd_mixer_load(handle)) < 0) {
		error("Mixer %s load error: %s", mix->card, snd_strerror(aux));
		goto clean_handle;
	}
	if (!(elem = snd_mixer_find_selem(handle, sid))) {
		error("Unable to find simple control %s,%i in card %s",
		      mix->name, mix->index, mix->card);
		goto clean_handle;
	}

	/* Get playback switch (0 = mute, 1 = unmute) */
	if (snd_mixer_selem_has_playback_switch(elem) &&
	    snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_MONO,
	                                        &aux) < 0) {
		error("Unable to get switch status from %s,%i in card %s",
		      mix->name, mix->index, mix->card);
		goto clean_handle;
	}
	/* Get volume */
	if (snd_mixer_selem_get_playback_volume_range(elem, &min, &max) < 0 ||
	    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO,
	                                        &vol) < 0) {
		error("Unable to get volume from %s,%i in card %s",
		      mix->name, mix->index, mix->card);
		goto clean_handle;
        }

	snd_mixer_close(handle);
	snd_mixer_selem_id_free(sid);

	/* Normalise volume and compute percentage */
	vol -= min;
	max -= min;
	if (max == 0)
		return NULL;
	vol = (long) ((double) vol / (double) max * 100);

	for (i = 0; i < LEN(volst); i++) {
		if (!strcmp(volst[i].state, s[aux]) && vol <= volst[i].perc) {
			esnprintf(buf, sizeof(buf), volst[i].fmt, vol);
			return buf;
		}
	}

clean_handle:
	snd_mixer_close(handle);
clean_sid:
	snd_mixer_selem_id_free(sid);

	return NULL;
}

const char *wifi_state(const void *rfdev)
{
	char *state;
	size_t i;

	if (!(state = rfkill_blocked(rfdev)))
		return NULL;

	for (i = 0; i < LEN(wifist); i++) {
		if (!strcmp(wifist[i].state, state))
			return wifist[i].fmt;
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	Display *dpy;
	char status[MAXLEN];
	const char *res;
	size_t len, i;
	int ret;

	/* Get status string */
	status[0] = '\0';
	for (i = len = 0; i < LEN(args); i++) {
		if (!(res = args[i].func(args[i].arg)))
			res = unknown_str;
		if ((ret = esnprintf(status + len, sizeof(status) - len,
                                     args[i].fmt, res)) < 0)
			break;
		len += ret;
	}

	/* Show status string */
	if (argc > 1 && strcmp(argv[1], "set") == 0) {
		if (!(dpy = XOpenDisplay(NULL))) {
			error("XOpenDisplay: Failed to open display");
			exit(EXIT_FAILURE);
		}
		if (XStoreName(dpy, DefaultRootWindow(dpy), status) < 0)
			error("XStoreName: Allocation failed");
		else
			XFlush(dpy);
		if (XCloseDisplay(dpy) < 0) {
			error("XCloseDisplay: Failed to close display");
			exit(EXIT_FAILURE);
		}
	} else {
		puts(status);
		fflush(stdout);
		if (ferror(stdout)) {
			error("puts: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

