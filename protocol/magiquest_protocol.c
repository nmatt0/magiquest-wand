#include "magiquest_protocol.h"

void magiquest_encode_signal(uint32_t wand_id, uint16_t magnitude, uint32_t* timings) {
    // Build the 56-bit frame: 8 zero preamble + 32-bit wand ID + 16-bit magnitude
    // Bits are transmitted MSB first within each field

    size_t idx = 0;

    // 8-bit preamble (all zeros)
    for(int i = 0; i < MAGIQUEST_PREAMBLE_BITS; i++) {
        timings[idx++] = MAGIQUEST_ZERO_MARK;
        if(idx < MAGIQUEST_TIMINGS_COUNT) {
            timings[idx++] = MAGIQUEST_ZERO_SPACE;
        }
    }

    // 32-bit wand ID, MSB first
    for(int i = 31; i >= 0; i--) {
        uint8_t bit = (wand_id >> i) & 1;
        if(bit) {
            timings[idx++] = MAGIQUEST_ONE_MARK;
            if(idx < MAGIQUEST_TIMINGS_COUNT) {
                timings[idx++] = MAGIQUEST_ONE_SPACE;
            }
        } else {
            timings[idx++] = MAGIQUEST_ZERO_MARK;
            if(idx < MAGIQUEST_TIMINGS_COUNT) {
                timings[idx++] = MAGIQUEST_ZERO_SPACE;
            }
        }
    }

    // 16-bit magnitude, MSB first
    for(int i = 15; i >= 0; i--) {
        uint8_t bit = (magnitude >> i) & 1;
        if(bit) {
            timings[idx++] = MAGIQUEST_ONE_MARK;
            if(idx < MAGIQUEST_TIMINGS_COUNT) {
                timings[idx++] = MAGIQUEST_ONE_SPACE;
            }
        } else {
            timings[idx++] = MAGIQUEST_ZERO_MARK;
            if(idx < MAGIQUEST_TIMINGS_COUNT) {
                timings[idx++] = MAGIQUEST_ZERO_SPACE;
            }
        }
    }
}

static bool magiquest_timing_match(uint32_t actual, uint32_t expected) {
    if(actual > expected) {
        return (actual - expected) <= MAGIQUEST_DECODE_TOLERANCE;
    } else {
        return (expected - actual) <= MAGIQUEST_DECODE_TOLERANCE;
    }
}

bool magiquest_decode_raw_signal(
    const uint32_t* timings,
    size_t count,
    uint32_t* out_wand_id,
    uint16_t* out_magnitude) {
    // We expect 111 timings (56 marks + 55 spaces), but accept 110-112
    if(count < (MAGIQUEST_TIMINGS_COUNT - 1) || count > (MAGIQUEST_TIMINGS_COUNT + 1)) {
        return false;
    }

    uint64_t frame = 0;
    size_t bits_decoded = 0;

    for(size_t i = 0; i < count && bits_decoded < MAGIQUEST_TOTAL_BITS; i += 2) {
        uint32_t mark = timings[i];
        uint32_t space;

        if(i + 1 < count) {
            space = timings[i + 1];
        } else {
            // Last bit — no trailing space. Decide by mark duration alone.
            uint8_t bit = magiquest_timing_match(mark, MAGIQUEST_ONE_MARK) ? 1 : 0;
            frame = (frame << 1) | bit;
            bits_decoded++;
            break;
        }

        uint32_t total = mark + space;

        // Verify total period is roughly correct
        if(!magiquest_timing_match(total, MAGIQUEST_BIT_PERIOD)) {
            return false;
        }

        // Decode: mark > 1/3 of total → 1, else 0
        uint8_t bit = (mark * 3 > total) ? 1 : 0;
        frame = (frame << 1) | bit;
        bits_decoded++;
    }

    if(bits_decoded != MAGIQUEST_TOTAL_BITS) {
        return false;
    }

    // Verify preamble is all zeros
    uint8_t preamble = (frame >> 48) & 0xFF;
    if(preamble != 0x00) {
        return false;
    }

    *out_wand_id = (frame >> 16) & 0xFFFFFFFF;
    *out_magnitude = frame & 0xFFFF;

    return true;
}
