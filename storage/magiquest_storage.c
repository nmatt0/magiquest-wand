#include "magiquest_storage.h"
#include <flipper_format/flipper_format.h>

bool magiquest_storage_load(MagiQuestApp* app) {
    furi_assert(app);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);

    app->wand_count = 0;

    bool success = false;
    do {
        if(!flipper_format_file_open_existing(ff, MAGIQUEST_STORAGE_FILE)) break;

        FuriString* filetype = furi_string_alloc();
        uint32_t version = 0;

        if(!flipper_format_read_header(ff, filetype, &version)) {
            furi_string_free(filetype);
            break;
        }

        if(furi_string_cmp_str(filetype, MAGIQUEST_STORAGE_FILETYPE) != 0 ||
           version != MAGIQUEST_STORAGE_VERSION) {
            furi_string_free(filetype);
            break;
        }
        furi_string_free(filetype);

        FuriString* name = furi_string_alloc();
        uint8_t id_bytes[4];
        uint8_t mag_bytes[2];

        while(app->wand_count < MAGIQUEST_MAX_WANDS) {
            if(!flipper_format_read_string(ff, "Name", name)) break;

            uint32_t id_count = 4;
            if(!flipper_format_read_hex(ff, "WandID", id_bytes, id_count)) break;

            uint32_t mag_count = 2;
            if(!flipper_format_read_hex(ff, "Magnitude", mag_bytes, mag_count)) break;

            MagiQuestWand* wand = &app->wands[app->wand_count];
            strncpy(wand->name, furi_string_get_cstr(name), MAGIQUEST_NAME_MAX_LEN - 1);
            wand->name[MAGIQUEST_NAME_MAX_LEN - 1] = '\0';

            wand->wand_id = ((uint32_t)id_bytes[0] << 24) |
                            ((uint32_t)id_bytes[1] << 16) |
                            ((uint32_t)id_bytes[2] << 8) |
                            (uint32_t)id_bytes[3];

            wand->magnitude = ((uint16_t)mag_bytes[0] << 8) | mag_bytes[1];

            app->wand_count++;
        }

        furi_string_free(name);
        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return success;
}

bool magiquest_storage_save(MagiQuestApp* app) {
    furi_assert(app);

    Storage* storage = furi_record_open(RECORD_STORAGE);

    // Ensure directory exists
    storage_simply_mkdir(storage, MAGIQUEST_STORAGE_DIR);

    FlipperFormat* ff = flipper_format_file_alloc(storage);

    bool success = false;
    do {
        if(!flipper_format_file_open_always(ff, MAGIQUEST_STORAGE_FILE)) break;

        if(!flipper_format_write_header_cstr(ff, MAGIQUEST_STORAGE_FILETYPE, MAGIQUEST_STORAGE_VERSION))
            break;

        bool write_ok = true;
        for(uint8_t i = 0; i < app->wand_count && write_ok; i++) {
            MagiQuestWand* wand = &app->wands[i];

            uint8_t id_bytes[4] = {
                (wand->wand_id >> 24) & 0xFF,
                (wand->wand_id >> 16) & 0xFF,
                (wand->wand_id >> 8) & 0xFF,
                wand->wand_id & 0xFF,
            };

            uint8_t mag_bytes[2] = {
                (wand->magnitude >> 8) & 0xFF,
                wand->magnitude & 0xFF,
            };

            if(!flipper_format_write_string_cstr(ff, "Name", wand->name)) {
                write_ok = false;
                break;
            }
            if(!flipper_format_write_hex(ff, "WandID", id_bytes, 4)) {
                write_ok = false;
                break;
            }
            if(!flipper_format_write_hex(ff, "Magnitude", mag_bytes, 2)) {
                write_ok = false;
                break;
            }
        }

        if(!write_ok) break;
        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return success;
}
