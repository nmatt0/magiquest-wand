#include "../magiquest_wand.h"
#include "magiquest_scene.h"
#include <infrared_transmit.h>

static void magiquest_scene_transmit_popup_callback(void* context) {
    MagiQuestApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, MagiQuestCustomEventTransmitDone);
}

void magiquest_scene_transmit_on_enter(void* context) {
    MagiQuestApp* app = context;

    if(app->selected_wand_index >= app->wand_count) return;

    MagiQuestWand* wand = &app->wands[app->selected_wand_index];

    // Encode the signal
    uint32_t timings[MAGIQUEST_TIMINGS_COUNT];
    magiquest_encode_signal(wand->wand_id, wand->magnitude, timings);

    // Transmit raw IR signal (blocking call)
    infrared_send_raw_ext(
        timings, MAGIQUEST_TIMINGS_COUNT, true, MAGIQUEST_CARRIER_FREQ, MAGIQUEST_DUTY_CYCLE);

    // Show confirmation popup
    popup_reset(app->popup);
    popup_set_header(app->popup, "Transmitted!", 64, 20, AlignCenter, AlignCenter);

    FuriString* body = furi_string_alloc();
    furi_string_printf(
        body,
        "%s\nID: %08lX  Mag: %04X",
        wand->name,
        (unsigned long)wand->wand_id,
        wand->magnitude);
    popup_set_text(app->popup, furi_string_get_cstr(body), 64, 40, AlignCenter, AlignCenter);
    furi_string_free(body);

    popup_set_timeout(app->popup, 1500);
    popup_set_callback(app->popup, magiquest_scene_transmit_popup_callback);
    popup_enable_timeout(app->popup);
    popup_set_context(app->popup, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewPopup);
}

bool magiquest_scene_transmit_on_event(void* context, SceneManagerEvent event) {
    MagiQuestApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagiQuestCustomEventTransmitDone) {
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void magiquest_scene_transmit_on_exit(void* context) {
    MagiQuestApp* app = context;
    popup_reset(app->popup);
}
