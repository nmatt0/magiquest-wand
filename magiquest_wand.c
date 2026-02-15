#include "magiquest_wand.h"
#include "scenes/magiquest_scene.h"
#include "storage/magiquest_storage.h"

static bool magiquest_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    MagiQuestApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool magiquest_back_event_callback(void* context) {
    furi_assert(context);
    MagiQuestApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static MagiQuestApp* magiquest_app_alloc(void) {
    MagiQuestApp* app = malloc(sizeof(MagiQuestApp));
    memset(app, 0, sizeof(MagiQuestApp));

    // Open GUI
    app->gui = furi_record_open(RECORD_GUI);

    // ViewDispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, magiquest_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, magiquest_back_event_callback);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // SceneManager
    app->scene_manager = scene_manager_alloc(&magiquest_scene_handlers, app);

    // Allocate GUI modules
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MagiQuestViewSubmenu, submenu_get_view(app->submenu));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MagiQuestViewTextInput, text_input_get_view(app->text_input));

    app->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MagiQuestViewByteInput, byte_input_get_view(app->byte_input));

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MagiQuestViewWidget, widget_get_view(app->widget));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MagiQuestViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    app->popup = popup_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MagiQuestViewPopup, popup_get_view(app->popup));

    app->number_input = number_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MagiQuestViewNumberInput, number_input_get_view(app->number_input));

    // Load saved wands
    magiquest_storage_load(app);

    return app;
}

static void magiquest_app_free(MagiQuestApp* app) {
    furi_assert(app);

    // Save wands before exit
    magiquest_storage_save(app);

    // Remove and free views
    view_dispatcher_remove_view(app->view_dispatcher, MagiQuestViewSubmenu);
    submenu_free(app->submenu);

    view_dispatcher_remove_view(app->view_dispatcher, MagiQuestViewTextInput);
    text_input_free(app->text_input);

    view_dispatcher_remove_view(app->view_dispatcher, MagiQuestViewByteInput);
    byte_input_free(app->byte_input);

    view_dispatcher_remove_view(app->view_dispatcher, MagiQuestViewWidget);
    widget_free(app->widget);

    view_dispatcher_remove_view(app->view_dispatcher, MagiQuestViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    view_dispatcher_remove_view(app->view_dispatcher, MagiQuestViewPopup);
    popup_free(app->popup);

    view_dispatcher_remove_view(app->view_dispatcher, MagiQuestViewNumberInput);
    number_input_free(app->number_input);

    // Free scene manager and view dispatcher
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t magiquest_wand_app(void* p) {
    UNUSED(p);

    MagiQuestApp* app = magiquest_app_alloc();

    scene_manager_next_scene(app->scene_manager, MagiQuestSceneMainMenu);
    view_dispatcher_run(app->view_dispatcher);

    magiquest_app_free(app);

    return 0;
}
