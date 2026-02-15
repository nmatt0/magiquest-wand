#include "../magiquest_wand.h"
#include "magiquest_scene.h"

static void magiquest_scene_wand_list_submenu_callback(void* context, uint32_t index) {
    MagiQuestApp* app = context;
    app->selected_wand_index = index;
    view_dispatcher_send_custom_event(app->view_dispatcher, MagiQuestCustomEventWandSelected);
}

void magiquest_scene_wand_list_on_enter(void* context) {
    MagiQuestApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Saved Wands");

    if(app->wand_count == 0) {
        submenu_add_item(app->submenu, "(no wands saved)", 0, NULL, NULL);
    } else {
        for(uint8_t i = 0; i < app->wand_count; i++) {
            submenu_add_item(
                app->submenu,
                app->wands[i].name,
                i,
                magiquest_scene_wand_list_submenu_callback,
                app);
        }
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewSubmenu);
}

bool magiquest_scene_wand_list_on_event(void* context, SceneManagerEvent event) {
    MagiQuestApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagiQuestCustomEventWandSelected) {
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneWandDetail);
            consumed = true;
        }
    }

    return consumed;
}

void magiquest_scene_wand_list_on_exit(void* context) {
    MagiQuestApp* app = context;
    submenu_reset(app->submenu);
}
