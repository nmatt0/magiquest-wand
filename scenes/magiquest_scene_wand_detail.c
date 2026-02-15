#include "../magiquest_wand.h"
#include "magiquest_scene.h"

// Scene states
#define DETAIL_STATE_VIEW 0
#define DETAIL_STATE_NAME 1 // TextInput for save-name

static void magiquest_scene_wand_detail_button_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    MagiQuestApp* app = context;
    if(type != InputTypeShort) return;

    if(result == GuiButtonTypeLeft) {
        if(app->wand_is_unsaved) {
            view_dispatcher_send_custom_event(
                app->view_dispatcher, MagiQuestCustomEventDetailSave);
        } else {
            view_dispatcher_send_custom_event(
                app->view_dispatcher, MagiQuestCustomEventDetailDelete);
        }
    } else if(result == GuiButtonTypeCenter) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, MagiQuestCustomEventDetailTransmit);
    } else if(result == GuiButtonTypeRight) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, MagiQuestCustomEventDetailConfig);
    }
}

static void magiquest_scene_wand_detail_text_input_callback(void* context) {
    MagiQuestApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, MagiQuestCustomEventTextInputDone);
}

static void magiquest_scene_wand_detail_build_widget(MagiQuestApp* app) {
    MagiQuestWand* wand = &app->wands[app->selected_wand_index];

    widget_reset(app->widget);

    FuriString* info = furi_string_alloc();
    furi_string_printf(
        info,
        "%s%s\nID: %08lX\nMag: 0x%04X (%u)",
        wand->name,
        app->wand_is_unsaved ? " *" : "",
        (unsigned long)wand->wand_id,
        wand->magnitude,
        wand->magnitude);
    widget_add_string_multiline_element(
        app->widget, 0, 2, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(info));
    furi_string_free(info);

    widget_add_button_element(
        app->widget,
        GuiButtonTypeLeft,
        app->wand_is_unsaved ? "Save" : "Delete",
        magiquest_scene_wand_detail_button_callback,
        app);
    widget_add_button_element(
        app->widget,
        GuiButtonTypeCenter,
        "Send",
        magiquest_scene_wand_detail_button_callback,
        app);
    widget_add_button_element(
        app->widget,
        GuiButtonTypeRight,
        "Config",
        magiquest_scene_wand_detail_button_callback,
        app);
}

void magiquest_scene_wand_detail_on_enter(void* context) {
    MagiQuestApp* app = context;

    scene_manager_set_scene_state(app->scene_manager, MagiQuestSceneWandDetail, DETAIL_STATE_VIEW);
    magiquest_scene_wand_detail_build_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewWidget);
}

bool magiquest_scene_wand_detail_on_event(void* context, SceneManagerEvent event) {
    MagiQuestApp* app = context;
    bool consumed = false;
    uint32_t state = scene_manager_get_scene_state(app->scene_manager, MagiQuestSceneWandDetail);

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case MagiQuestCustomEventDetailSave:
            // Enter name editing for save
            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneWandDetail, DETAIL_STATE_NAME);
            strncpy(app->text_buf, app->wands[app->selected_wand_index].name,
                    MAGIQUEST_NAME_MAX_LEN - 1);
            app->text_buf[MAGIQUEST_NAME_MAX_LEN - 1] = '\0';
            text_input_reset(app->text_input);
            text_input_set_header_text(app->text_input, "Wand Name:");
            text_input_set_result_callback(
                app->text_input,
                magiquest_scene_wand_detail_text_input_callback,
                app,
                app->text_buf,
                MAGIQUEST_NAME_MAX_LEN,
                true);
            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewTextInput);
            consumed = true;
            break;

        case MagiQuestCustomEventTextInputDone:
            if(state == DETAIL_STATE_NAME) {
                // Commit the wand: copy name, increment count, clear unsaved
                MagiQuestWand* wand = &app->wands[app->selected_wand_index];
                strncpy(wand->name, app->text_buf, MAGIQUEST_NAME_MAX_LEN - 1);
                wand->name[MAGIQUEST_NAME_MAX_LEN - 1] = '\0';
                app->wand_count++;
                app->wand_is_unsaved = false;
                // Rebuild widget (now shows Delete instead of Save)
                scene_manager_set_scene_state(
                    app->scene_manager, MagiQuestSceneWandDetail, DETAIL_STATE_VIEW);
                magiquest_scene_wand_detail_build_widget(app);
                view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewWidget);
                consumed = true;
            }
            break;

        case MagiQuestCustomEventDetailTransmit:
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneTransmit);
            consumed = true;
            break;

        case MagiQuestCustomEventDetailConfig:
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneConfig);
            consumed = true;
            break;

        case MagiQuestCustomEventDetailDelete:
            if(app->selected_wand_index < app->wand_count) {
                for(uint8_t i = app->selected_wand_index; i < app->wand_count - 1; i++) {
                    app->wands[i] = app->wands[i + 1];
                }
                app->wand_count--;
                // Navigate back — try WandList first, fall back to MainMenu
                if(!scene_manager_search_and_switch_to_previous_scene(
                       app->scene_manager, MagiQuestSceneWandList)) {
                    scene_manager_search_and_switch_to_previous_scene(
                        app->scene_manager, MagiQuestSceneMainMenu);
                }
            }
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(state == DETAIL_STATE_NAME) {
            // Back from name input → return to widget
            scene_manager_set_scene_state(
                app->scene_manager, MagiQuestSceneWandDetail, DETAIL_STATE_VIEW);
            view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewWidget);
            consumed = true;
        } else if(app->wand_is_unsaved) {
            // Back from unsaved wand → discard and go back normally
            app->wand_is_unsaved = false;
        }
    }

    return consumed;
}

void magiquest_scene_wand_detail_on_exit(void* context) {
    MagiQuestApp* app = context;
    widget_reset(app->widget);
    text_input_reset(app->text_input);
}
