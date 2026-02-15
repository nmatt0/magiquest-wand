#include "../magiquest_wand.h"
#include "magiquest_scene.h"

// State machine within this scene: first text input (name), then byte input (wand ID)
// We use scene_manager_get_scene_state to track which phase we're in
#define WAND_ADD_PHASE_NAME 0
#define WAND_ADD_PHASE_ID   1

static void magiquest_scene_wand_add_text_input_callback(void* context) {
    MagiQuestApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, MagiQuestCustomEventTextInputDone);
}

static void magiquest_scene_wand_add_byte_input_callback(void* context) {
    MagiQuestApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, MagiQuestCustomEventByteInputDone);
}

void magiquest_scene_wand_add_on_enter(void* context) {
    MagiQuestApp* app = context;

    if(app->wand_count >= MAGIQUEST_MAX_WANDS) {
        // Show error via widget
        widget_reset(app->widget);
        widget_add_string_element(
            app->widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Max wands reached!");
        view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewWidget);
        return;
    }

    // Phase: Name entry
    scene_manager_set_scene_state(app->scene_manager, MagiQuestSceneWandAdd, WAND_ADD_PHASE_NAME);
    memset(app->text_buf, 0, sizeof(app->text_buf));

    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Wand Name:");
    text_input_set_result_callback(
        app->text_input,
        magiquest_scene_wand_add_text_input_callback,
        app,
        app->text_buf,
        MAGIQUEST_NAME_MAX_LEN,
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewTextInput);
}

bool magiquest_scene_wand_add_on_event(void* context, SceneManagerEvent event) {
    MagiQuestApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        uint32_t phase =
            scene_manager_get_scene_state(app->scene_manager, MagiQuestSceneWandAdd);

        if(event.event == MagiQuestCustomEventTextInputDone && phase == WAND_ADD_PHASE_NAME) {
            // Name entered, move to byte input for wand ID
            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneWandAdd, WAND_ADD_PHASE_ID);

            memset(app->byte_buf, 0, sizeof(app->byte_buf));
            byte_input_set_header_text(app->byte_input, "Wand ID (4 bytes):");
            byte_input_set_result_callback(
                app->byte_input, magiquest_scene_wand_add_byte_input_callback, NULL, app, app->byte_buf, 4);

            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewByteInput);
            consumed = true;

        } else if(event.event == MagiQuestCustomEventByteInputDone && phase == WAND_ADD_PHASE_ID) {
            // Both name and ID entered — create the wand
            MagiQuestWand* wand = &app->wands[app->wand_count];
            strncpy(wand->name, app->text_buf, MAGIQUEST_NAME_MAX_LEN - 1);
            wand->name[MAGIQUEST_NAME_MAX_LEN - 1] = '\0';

            wand->wand_id = ((uint32_t)app->byte_buf[0] << 24) |
                            ((uint32_t)app->byte_buf[1] << 16) |
                            ((uint32_t)app->byte_buf[2] << 8) |
                            (uint32_t)app->byte_buf[3];

            wand->magnitude = 0x0441; // Default magnitude

            app->selected_wand_index = app->wand_count;
            app->wand_count++;

            // Go to wand detail, replacing this scene in the stack
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneWandDetail);
            consumed = true;
        }
    }

    return consumed;
}

void magiquest_scene_wand_add_on_exit(void* context) {
    MagiQuestApp* app = context;
    text_input_reset(app->text_input);
    byte_input_set_result_callback(app->byte_input, NULL, NULL, NULL, NULL, 0);
    widget_reset(app->widget);
}
