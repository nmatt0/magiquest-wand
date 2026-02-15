#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/popup.h>
#include <gui/modules/number_input.h>
#include <storage/storage.h>
#include <infrared/worker/infrared_worker.h>

#include "protocol/magiquest_protocol.h"

#define MAGIQUEST_MAX_WANDS    16
#define MAGIQUEST_NAME_MAX_LEN 32

typedef struct {
    char name[MAGIQUEST_NAME_MAX_LEN];
    uint32_t wand_id;
    uint16_t magnitude;
} MagiQuestWand;

// Scene IDs
typedef enum {
    MagiQuestSceneMainMenu,
    MagiQuestSceneWandList,
    MagiQuestSceneWandAdd,
    MagiQuestSceneWandCapture,
    MagiQuestSceneWandDetail,
    MagiQuestSceneConfig,
    MagiQuestSceneTransmit,
    MagiQuestSceneCount,
} MagiQuestScene;

// View IDs
typedef enum {
    MagiQuestViewSubmenu,
    MagiQuestViewTextInput,
    MagiQuestViewByteInput,
    MagiQuestViewWidget,
    MagiQuestViewVariableItemList,
    MagiQuestViewPopup,
    MagiQuestViewNumberInput,
} MagiQuestView;

// Custom events
typedef enum {
    MagiQuestCustomEventMainMenuSavedWands,
    MagiQuestCustomEventMainMenuAddWand,
    MagiQuestCustomEventMainMenuCaptureWand,
    MagiQuestCustomEventTextInputDone,
    MagiQuestCustomEventByteInputDone,
    MagiQuestCustomEventWandSelected,
    MagiQuestCustomEventDetailTransmit,
    MagiQuestCustomEventDetailSave,
    MagiQuestCustomEventDetailDelete,
    MagiQuestCustomEventDetailConfig,
    MagiQuestCustomEventTransmitDone,
    MagiQuestCustomEventCaptureDecoded,
    MagiQuestCustomEventConfigEditId,
    MagiQuestCustomEventConfigEditMag,
    MagiQuestCustomEventNumberInputDone,
} MagiQuestCustomEvent;

typedef struct {
    // Core
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    // GUI modules
    Submenu* submenu;
    TextInput* text_input;
    ByteInput* byte_input;
    Widget* widget;
    VariableItemList* variable_item_list;
    Popup* popup;
    NumberInput* number_input;

    // Wand data
    MagiQuestWand wands[MAGIQUEST_MAX_WANDS];
    uint8_t wand_count;
    uint8_t selected_wand_index;
    bool wand_is_unsaved; // true when captured but not yet committed

    // Temp buffers for input
    char text_buf[MAGIQUEST_NAME_MAX_LEN];
    uint8_t byte_buf[4]; // For wand ID entry (4 bytes = 32 bits)

    // IR worker for capture
    InfraredWorker* ir_worker;
} MagiQuestApp;
