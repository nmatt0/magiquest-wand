#include "../magiquest_wand.h"
#include "magiquest_scene.h"

static void magiquest_scene_wand_capture_ir_callback(
    void* context,
    InfraredWorkerSignal* signal) {
    MagiQuestApp* app = context;

    if(infrared_worker_signal_is_decoded(signal)) {
        return;
    }

    const uint32_t* timings;
    size_t timings_count;
    infrared_worker_get_raw_signal(signal, &timings, &timings_count);

    uint32_t wand_id = 0;
    uint16_t magnitude = 0;

    if(magiquest_decode_raw_signal(timings, timings_count, &wand_id, &magnitude)) {
        if(app->wand_count < MAGIQUEST_MAX_WANDS) {
            // Stage the wand at wands[wand_count] but do NOT increment wand_count yet
            MagiQuestWand* wand = &app->wands[app->wand_count];
            snprintf(wand->name, MAGIQUEST_NAME_MAX_LEN, "Wand_%08lX", (unsigned long)wand_id);
            wand->wand_id = wand_id;
            wand->magnitude = magnitude;
            app->selected_wand_index = app->wand_count;
            app->wand_is_unsaved = true;

            view_dispatcher_send_custom_event(
                app->view_dispatcher, MagiQuestCustomEventCaptureDecoded);
        }
    }
}

void magiquest_scene_wand_capture_on_enter(void* context) {
    MagiQuestApp* app = context;

    widget_reset(app->widget);
    widget_add_string_multiline_element(
        app->widget,
        64,
        20,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        "Cast your wand\nat the Flipper!\n\nWaiting for IR...");

    view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewWidget);

    app->ir_worker = infrared_worker_alloc();
    infrared_worker_rx_enable_signal_decoding(app->ir_worker, false);
    infrared_worker_rx_enable_blink_on_receiving(app->ir_worker, true);
    infrared_worker_rx_set_received_signal_callback(
        app->ir_worker, magiquest_scene_wand_capture_ir_callback, app);
    infrared_worker_rx_start(app->ir_worker);
}

bool magiquest_scene_wand_capture_on_event(void* context, SceneManagerEvent event) {
    MagiQuestApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagiQuestCustomEventCaptureDecoded) {
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneWandDetail);
            consumed = true;
        }
    }

    return consumed;
}

void magiquest_scene_wand_capture_on_exit(void* context) {
    MagiQuestApp* app = context;

    if(app->ir_worker) {
        infrared_worker_rx_stop(app->ir_worker);
        infrared_worker_free(app->ir_worker);
        app->ir_worker = NULL;
    }

    widget_reset(app->widget);
}
