#include "../magiquest_wand.h"
#include "magiquest_scene.h"

static void magiquest_scene_main_menu_submenu_callback(void* context, uint32_t index) {
    MagiQuestApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void magiquest_scene_main_menu_on_enter(void* context) {
    MagiQuestApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "MagiQuest Wand");

    submenu_add_item(
        app->submenu,
        "Saved Wands",
        MagiQuestCustomEventMainMenuSavedWands,
        magiquest_scene_main_menu_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Add Wand",
        MagiQuestCustomEventMainMenuAddWand,
        magiquest_scene_main_menu_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Capture Wand",
        MagiQuestCustomEventMainMenuCaptureWand,
        magiquest_scene_main_menu_submenu_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, MagiQuestViewSubmenu);
}

bool magiquest_scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    MagiQuestApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case MagiQuestCustomEventMainMenuSavedWands:
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneWandList);
            consumed = true;
            break;
        case MagiQuestCustomEventMainMenuAddWand:
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneWandAdd);
            consumed = true;
            break;
        case MagiQuestCustomEventMainMenuCaptureWand:
            scene_manager_next_scene(app->scene_manager, MagiQuestSceneWandCapture);
            consumed = true;
            break;
        }
    }

    return consumed;
}

void magiquest_scene_main_menu_on_exit(void* context) {
    MagiQuestApp* app = context;
    submenu_reset(app->submenu);
}
