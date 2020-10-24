#include <alsa/asoundlib.h>

#include "info.h"
#include "../util.h"

int volume_getinfo(struct volume_info *info, const struct mixer *mixer)
{
	long vol, min, max;
	int aux;
	snd_mixer_selem_id_t *sid;
	snd_mixer_t *handle;
	snd_mixer_elem_t *elem;

	if ((aux = snd_mixer_selem_id_malloc(&sid)) < 0) {
		error("Mixer allocation error: %s", snd_strerror(aux));
		return -1;
	}
	snd_mixer_selem_id_set_index(sid, mixer->index);
	snd_mixer_selem_id_set_name(sid, mixer->name);
	if ((aux = snd_mixer_open(&handle, 0)) < 0) {
		error("Mixer %s open error: %s", mixer->card, snd_strerror(aux));
		goto clean_sid;
	}
	if ((aux = snd_mixer_attach(handle, mixer->card)) < 0) {
		error("Mixer %s attach error: %s", mixer->card, snd_strerror(aux));
		goto clean_handle;
	}
	if ((aux = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
		error("Mixer %s register error: %s", mixer->card, snd_strerror(aux));
		goto clean_handle;
	}
 	if ((aux = snd_mixer_load(handle)) < 0) {
		error("Mixer %s load error: %s", mixer->card, snd_strerror(aux));
		goto clean_handle;
	}
	if (!(elem = snd_mixer_find_selem(handle, sid))) {
		error("Unable to find simple control %s,%i in card %s",
		      mixer->name, mixer->index, mixer->card);
		goto clean_handle;
	}

	/* Get playback switch (0 = mute, 1 = unmute) */
	if (snd_mixer_selem_has_playback_switch(elem) &&
	    snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_MONO, &aux) < 0) {
		error("Unable to get switch status from %s,%i in card %s",
		      mixer->name, mixer->index, mixer->card);
		goto clean_handle;
	}
	/* Get volume */
	if (snd_mixer_selem_get_playback_volume_range(elem, &min, &max) < 0 ||
	    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &vol) < 0) {
		error("Unable to get volume from %s,%i in card %s",
		      mixer->name, mixer->index, mixer->card);
		goto clean_handle;
        }

	snd_mixer_close(handle);
	snd_mixer_selem_id_free(sid);

	/* Normalise volume */
	vol -= min;
	max -= min;
	if (max == 0)
		return -1;

	info->perc = (100 * vol) / max;
	info->status = (aux) ? Unmute : Mute;

	return 0;

clean_handle:
	snd_mixer_close(handle);
clean_sid:
	snd_mixer_selem_id_free(sid);

	return -1;
}
