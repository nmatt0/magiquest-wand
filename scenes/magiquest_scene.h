#pragma once

#include <gui/scene_manager.h>

// Scene handler declarations - each scene implements on_enter, on_event, on_exit

void magiquest_scene_main_menu_on_enter(void* context);
bool magiquest_scene_main_menu_on_event(void* context, SceneManagerEvent event);
void magiquest_scene_main_menu_on_exit(void* context);

void magiquest_scene_wand_list_on_enter(void* context);
bool magiquest_scene_wand_list_on_event(void* context, SceneManagerEvent event);
void magiquest_scene_wand_list_on_exit(void* context);

void magiquest_scene_wand_add_on_enter(void* context);
bool magiquest_scene_wand_add_on_event(void* context, SceneManagerEvent event);
void magiquest_scene_wand_add_on_exit(void* context);

void magiquest_scene_wand_capture_on_enter(void* context);
bool magiquest_scene_wand_capture_on_event(void* context, SceneManagerEvent event);
void magiquest_scene_wand_capture_on_exit(void* context);

void magiquest_scene_wand_detail_on_enter(void* context);
bool magiquest_scene_wand_detail_on_event(void* context, SceneManagerEvent event);
void magiquest_scene_wand_detail_on_exit(void* context);

void magiquest_scene_config_on_enter(void* context);
bool magiquest_scene_config_on_event(void* context, SceneManagerEvent event);
void magiquest_scene_config_on_exit(void* context);

void magiquest_scene_transmit_on_enter(void* context);
bool magiquest_scene_transmit_on_event(void* context, SceneManagerEvent event);
void magiquest_scene_transmit_on_exit(void* context);

extern const SceneManagerHandlers magiquest_scene_handlers;
