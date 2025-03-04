#include <pulse/pulseaudio.h>

#include "info.h"
#include "../util.h"

static void context_state_callback(pa_context *c, void *userdata)
{
        int *ready = userdata;

        if (!c) {
                error("context_state_callback(): Null pa_context");
                *ready = -1;
                return;
        }

        switch (pa_context_get_state(c)) {
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
                break;
        case PA_CONTEXT_READY:
                *ready = 2;
                break;
        case PA_CONTEXT_TERMINATED:
                *ready = 0;
                break;
        case PA_CONTEXT_FAILED:
        default:
                error("Connection failure: %s",
                      pa_strerror(pa_context_errno(c)));
                *ready = -1;
                break;
        }
}

static void get_sink_info_callback(pa_context *c, const pa_sink_info *i,
                                   int is_last, void *userdata)
{
        struct ps_info *info = (struct ps_info *) userdata;

        if (is_last < 0) {
                error("Failed to get sink information: %s",
                      pa_strerror(pa_context_errno(c)));
                info->status = -1;
                return;
        }
        if (is_last)
                return;
        if (!i) {
                error("get_sink_info_callback(): Null pa_sink_info");
                info->status = -1;
                return;
        }

        info->perc = ((uint64_t) pa_cvolume_avg(&(i->volume)) * 100 +
                      (uint64_t) PA_VOLUME_NORM / 2) /
                     (uint64_t) PA_VOLUME_NORM;
        info->status = (i->mute) ? Mute : Unmute;
}

int volume_pulse_getinfo(struct ps_info *info, const char *sink)
{
        int ret = 1;
        int ready = 1;
        int state = 0;
        pa_mainloop *ml = NULL;
        pa_mainloop_api *mlapi = NULL;
        pa_context *ctx = NULL;
        pa_operation *op = NULL;

        if (!(ml = pa_mainloop_new())) {
                error("pa_mainloop_new() failed.");
                goto quit;
        }
        mlapi = pa_mainloop_get_api(ml);
        if (!(ctx = pa_context_new(mlapi, "tstat"))) {
                error("pa_context_new() failed.");
                goto quit_ml;
        }
        if (pa_context_connect(ctx, NULL, 0, NULL) < 0) {
                error("pa_context_connect() failed: %s",
                       pa_strerror(pa_context_errno(ctx)));
                goto quit_ctx;
        }
        pa_context_set_state_callback(ctx, context_state_callback, &ready);

        info->status = 0;

        for (;;) {
                if (ready == 1) {
                        if (pa_mainloop_iterate(ml, 1, NULL) < 0) {
                                error("pa_mainloop_iterate() failed.");
                                goto quit_disc;
                        }
                        continue;
                } else if (ready <= 0) {
                        ret = (ready < 0);
                        goto quit_disc;
                }
                if (state == 0) {
                        op = pa_context_get_sink_info_by_name(ctx, sink,
                                 get_sink_info_callback, info);
                        state++;
                } else if (pa_operation_get_state(op) == PA_OPERATION_DONE) {
                        ret = (info->status < 0);
                        pa_operation_unref(op);
                        goto quit_disc;
                }
                if (pa_mainloop_iterate(ml, 1, NULL) < 0) {
                        error("pa_mainloop_iterate() failed.");
                        goto quit_disc;
                }
        }
quit_disc:
        pa_context_disconnect(ctx);
quit_ctx:
        pa_context_unref(ctx);
quit_ml:
        pa_mainloop_free(ml);
quit:
        return ret;
}
