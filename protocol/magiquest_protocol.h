#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// MagiQuest IR Protocol Constants
// 56 bits: 8-bit zero preamble + 32-bit wand ID + 16-bit magnitude
// Pulse-width encoding at 38kHz, 0.33 duty cycle

#define MAGIQUEST_CARRIER_FREQ   38000
#define MAGIQUEST_DUTY_CYCLE     0.33f

#define MAGIQUEST_PREAMBLE_BITS  8
#define MAGIQUEST_ID_BITS        32
#define MAGIQUEST_MAG_BITS       16
#define MAGIQUEST_TOTAL_BITS     56

// Timing constants (microseconds)
#define MAGIQUEST_ZERO_MARK      280
#define MAGIQUEST_ZERO_SPACE     840
#define MAGIQUEST_ONE_MARK       560
#define MAGIQUEST_ONE_SPACE      560

// Total period per bit is ~1120us for both 0 and 1
#define MAGIQUEST_BIT_PERIOD     1120

// Raw timings array: 56 bits * 2 (mark+space) - 1 (last bit has no trailing space) = 111
#define MAGIQUEST_TIMINGS_COUNT  111

// Decode tolerance (microseconds)
#define MAGIQUEST_DECODE_TOLERANCE 200

// Encode a wand_id + magnitude into a raw timings array.
// timings must point to an array of at least MAGIQUEST_TIMINGS_COUNT uint32_t elements.
void magiquest_encode_signal(uint32_t wand_id, uint16_t magnitude, uint32_t* timings);

// Decode a raw timings array back into wand_id + magnitude.
// Returns true on success, false if the signal doesn't match the protocol.
bool magiquest_decode_raw_signal(
    const uint32_t* timings,
    size_t count,
    uint32_t* out_wand_id,
    uint16_t* out_magnitude);
