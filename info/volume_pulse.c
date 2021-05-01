#include <pulse/pulseaudio.h>

#include "info.h"
#include "../util.h"

static char *sink_name = NULL;
static pa_mainloop_api *mainloop_api = NULL;

static void quit(int ret)
{
	mainloop_api->quit(mainloop_api, ret);
}
static void exit_signal_callback(pa_mainloop_api *m, pa_signal_event *e,
                                 int sig, void *userdata)
{
	quit(0);
}

static void get_sink_info_callback(pa_context *c, const pa_sink_info *i,
                                   int is_last, void *userdata)
{
	struct ps_info *info = (struct ps_info *) userdata;

	if (is_last < 0) {
		error("Failed to get sink information: %s",
		      pa_strerror(pa_context_errno(c)));
		quit(1);
		return;
	}
	if (is_last) {
		pa_context_disconnect(c);
		return;
	}
	if (i) {
		info->perc = ((uint64_t) pa_cvolume_avg(&(i->volume)) * 100 +
		              (uint64_t) PA_VOLUME_NORM / 2) /
		             (uint64_t) PA_VOLUME_NORM;
		info->status = (i->mute) ? Mute : Unmute;
	} else {
		quit(1);
	}
}

static void context_state_callback(pa_context *c, void *userdata)
{
	pa_operation *o = NULL;

	if (!c) {
		quit(1);
		return;
	}

	switch (pa_context_get_state(c)) {
	case PA_CONTEXT_CONNECTING:
	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;
	case PA_CONTEXT_READY:
		o = pa_context_get_sink_info_by_name(c, sink_name,
		                                     get_sink_info_callback,
		                                     userdata);
		if (o)
			pa_operation_unref(o);
		break;
	case PA_CONTEXT_TERMINATED:
		quit(0);
		break;
	case PA_CONTEXT_FAILED:
	default:
		error("Connection failure: %s",
		      pa_strerror(pa_context_errno(c)));
		quit(1);
	}
}

int volume_pulse_getinfo(struct ps_info *info, const char *sink)
{
	int ret = 1;
	pa_mainloop *m = NULL;
	pa_context *context = NULL;

	sink_name = sink;

	if (!(m = pa_mainloop_new())) {
		error("pa_mainloop_new() failed.");
		goto quit;
	}
	mainloop_api = pa_mainloop_get_api(m);
	if (!(context = pa_context_new(mainloop_api, NULL))) {
		error("pa_context_new() failed.");
		goto quit;
	}
	pa_context_set_state_callback(context, context_state_callback, info);
	if (pa_context_connect(context, NULL, 0, NULL) < 0) {
		error("pa_context_connect() failed: %s",
		       pa_strerror(pa_context_errno(context)));
		goto quit;
	}
	if (pa_mainloop_run(m, &ret) < 0) {
		error("pa_mainloop_run() failed.");
		goto quit;
	}

quit:
	if (context)
		pa_context_unref(context);
	
	if (m)
		pa_mainloop_free(m);

	return ret;
}
