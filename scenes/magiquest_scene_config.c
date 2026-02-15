#include "../magiquest_wand.h"
#include "magiquest_scene.h"

// Scene states
#define CONFIG_STATE_LIST 0
#define CONFIG_STATE_ID   1 // ByteInput for wand ID
#define CONFIG_STATE_MAG  2 // NumberInput for magnitude

static void magiquest_scene_config_enter_callback(void* context, uint32_t index) {
    MagiQuestApp* app = context;
    if(index == 0) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, MagiQuestCustomEventConfigEditMag);
    } else if(index == 1) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, MagiQuestCustomEventConfigEditId);
    }
}

static void magiquest_scene_config_byte_input_callback(void* context) {
    MagiQuestApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, MagiQuestCustomEventByteInputDone);
}

static void magiquest_scene_config_number_input_callback(void* context, int32_t number) {
    MagiQuestApp* app = context;
    app->wands[app->selected_wand_index].magnitude = (uint16_t)number;
    view_dispatcher_send_custom_event(
        app->view_dispatcher, MagiQuestCustomEventNumberInputDone);
}

static void magiquest_scene_config_build_list(MagiQuestApp* app) {
    MagiQuestWand* wand = &app->wands[app->selected_wand_index];

    variable_item_list_reset(app->variable_item_list);

    // Item 0: Magnitude (click to edit via NumberInput)
    VariableItem* mag_item = variable_item_list_add(
        app->variable_item_list, "Magnitude", 1, NULL, NULL);
    FuriString* mag_str = furi_string_alloc();
    furi_string_printf(mag_str, "%u", wand->magnitude);
    variable_item_set_current_value_text(mag_item, furi_string_get_cstr(mag_str));
    furi_string_free(mag_str);

    // Item 1: Wand ID (click to edit via ByteInput)
    VariableItem* id_item = variable_item_list_add(
        app->variable_item_list, "Wand ID", 1, NULL, NULL);
    FuriString* id_str = furi_string_alloc();
    furi_string_printf(id_str, "%08lX", (unsigned long)wand->wand_id);
    variable_item_set_current_value_text(id_item, furi_string_get_cstr(id_str));
    furi_string_free(id_str);

    variable_item_list_set_enter_callback(
        app->variable_item_list, magiquest_scene_config_enter_callback, app);
}

void magiquest_scene_config_on_enter(void* context) {
    MagiQuestApp* app = context;

    scene_manager_set_scene_state(app->scene_manager, MagiQuestSceneConfig, CONFIG_STATE_LIST);
    magiquest_scene_config_build_list(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewVariableItemList);
}

bool magiquest_scene_config_on_event(void* context, SceneManagerEvent event) {
    MagiQuestApp* app = context;
    bool consumed = false;
    uint32_t state = scene_manager_get_scene_state(app->scene_manager, MagiQuestSceneConfig);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagiQuestCustomEventConfigEditMag) {
            // Open NumberInput for magnitude
            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneConfig, CONFIG_STATE_MAG);

            MagiQuestWand* wand = &app->wands[app->selected_wand_index];
            number_input_set_header_text(app->number_input, "Magnitude (0-65535):");
            number_input_set_result_callback(
                app->number_input,
                magiquest_scene_config_number_input_callback,
                app,
                wand->magnitude,
                0,
                65535);
            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewNumberInput);
            consumed = true;

        } else if(event.event == MagiQuestCustomEventNumberInputDone && state == CONFIG_STATE_MAG) {
            // Magnitude already updated in callback, return to list
            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneConfig, CONFIG_STATE_LIST);
            magiquest_scene_config_build_list(app);
            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewVariableItemList);
            consumed = true;

        } else if(event.event == MagiQuestCustomEventConfigEditId) {
            // Open ByteInput for wand ID
            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneConfig, CONFIG_STATE_ID);

            MagiQuestWand* wand = &app->wands[app->selected_wand_index];
            app->byte_buf[0] = (wand->wand_id >> 24) & 0xFF;
            app->byte_buf[1] = (wand->wand_id >> 16) & 0xFF;
            app->byte_buf[2] = (wand->wand_id >> 8) & 0xFF;
            app->byte_buf[3] = wand->wand_id & 0xFF;

            byte_input_set_header_text(app->byte_input, "Wand ID (4 bytes):");
            byte_input_set_result_callback(
                app->byte_input,
                magiquest_scene_config_byte_input_callback,
                NULL,
                app,
                app->byte_buf,
                4);
            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewByteInput);
            consumed = true;

        } else if(event.event == MagiQuestCustomEventByteInputDone && state == CONFIG_STATE_ID) {
            // Update wand ID from byte buffer
            MagiQuestWand* wand = &app->wands[app->selected_wand_index];
            wand->wand_id = ((uint32_t)app->byte_buf[0] << 24) |
                            ((uint32_t)app->byte_buf[1] << 16) |
                            ((uint32_t)app->byte_buf[2] << 8) |
                            (uint32_t)app->byte_buf[3];

            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneConfig, CONFIG_STATE_LIST);
            magiquest_scene_config_build_list(app);
            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewVariableItemList);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(state != CONFIG_STATE_LIST) {
            // Back from any sub-editor → return to config list
            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneConfig, CONFIG_STATE_LIST);
            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewVariableItemList);
            consumed = true;
        }
    }

    return consumed;
}

void magiquest_scene_config_on_exit(void* context) {
    MagiQuestApp* app = context;
    variable_item_list_reset(app->variable_item_list);
    byte_input_set_result_callback(app->byte_input, NULL, NULL, NULL, NULL, 0);
}
