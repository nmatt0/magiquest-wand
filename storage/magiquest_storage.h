#pragma once

#include "../magiquest_wand.h"

#define MAGIQUEST_STORAGE_DIR  EXT_PATH("apps_data/magiquest_wand")
#define MAGIQUEST_STORAGE_FILE EXT_PATH("apps_data/magiquest_wand/wands.txt")

#define MAGIQUEST_STORAGE_FILETYPE "MagiQuest Wands"
#define MAGIQUEST_STORAGE_VERSION   1

bool magiquest_storage_load(MagiQuestApp* app);
bool magiquest_storage_save(MagiQuestApp* app);
