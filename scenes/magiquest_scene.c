#include "magiquest_scene.h"
#include "../magiquest_wand.h"

// Handler table — order must match MagiQuestScene enum

const SceneManagerHandlers magiquest_scene_handlers = {
    .scene_num = MagiQuestSceneCount,
    .on_enter_handlers =
        (AppSceneOnEnterCallback[]){
            [MagiQuestSceneMainMenu] = magiquest_scene_main_menu_on_enter,
            [MagiQuestSceneWandList] = magiquest_scene_wand_list_on_enter,
            [MagiQuestSceneWandAdd] = magiquest_scene_wand_add_on_enter,
            [MagiQuestSceneWandCapture] = magiquest_scene_wand_capture_on_enter,
            [MagiQuestSceneWandDetail] = magiquest_scene_wand_detail_on_enter,
            [MagiQuestSceneConfig] = magiquest_scene_config_on_enter,
            [MagiQuestSceneTransmit] = magiquest_scene_transmit_on_enter,
        },
    .on_event_handlers =
        (AppSceneOnEventCallback[]){
            [MagiQuestSceneMainMenu] = magiquest_scene_main_menu_on_event,
            [MagiQuestSceneWandList] = magiquest_scene_wand_list_on_event,
            [MagiQuestSceneWandAdd] = magiquest_scene_wand_add_on_event,
            [MagiQuestSceneWandCapture] = magiquest_scene_wand_capture_on_event,
            [MagiQuestSceneWandDetail] = magiquest_scene_wand_detail_on_event,
            [MagiQuestSceneConfig] = magiquest_scene_config_on_event,
            [MagiQuestSceneTransmit] = magiquest_scene_transmit_on_event,
        },
    .on_exit_handlers =
        (AppSceneOnExitCallback[]){
            [MagiQuestSceneMainMenu] = magiquest_scene_main_menu_on_exit,
            [MagiQuestSceneWandList] = magiquest_scene_wand_list_on_exit,
            [MagiQuestSceneWandAdd] = magiquest_scene_wand_add_on_exit,
            [MagiQuestSceneWandCapture] = magiquest_scene_wand_capture_on_exit,
            [MagiQuestSceneWandDetail] = magiquest_scene_wand_detail_on_exit,
            [MagiQuestSceneConfig] = magiquest_scene_config_on_exit,
            [MagiQuestSceneTransmit] = magiquest_scene_transmit_on_exit,
        },
};
