# magiquest-wand

A Flipper Zero application for capturing, storing, and replaying [MagiQuest](https://en.wikipedia.org/wiki/MagiQuest) wand IR signals.

MagiQuest is an interactive theme park attraction where guests wave infrared wands to cast spells at targets throughout the venue. Each wand transmits a unique 32-bit ID over IR using a proprietary pulse-width encoded protocol. This app lets you capture wand IDs, save them, edit their parameters, and replay them from your Flipper Zero.

## Features

- **Capture** — Point a MagiQuest wand at the Flipper's IR receiver to decode and save its signal
- **Manual Add** — Enter a wand ID by hand (name + 4-byte hex ID)
- **Transmit** — Replay any saved wand signal through the Flipper's IR emitter
- **Configure** — Edit wand ID and magnitude values for saved wands
- **Persistent Storage** — Wands are saved to SD card and persist across reboots (up to 16 wands)

## Protocol

MagiQuest wands transmit a 56-bit pulse-width encoded frame at 38 kHz, 33% duty cycle:

| Field     | Bits | Description                    |
|-----------|------|--------------------------------|
| Preamble  | 8    | All zeros                      |
| Wand ID   | 32   | Unique wand identifier         |
| Magnitude | 16   | Signal strength / spell power  |

Bit encoding uses mark/space timing within a fixed ~1120 µs period:
- **0** — 280 µs mark, 840 µs space
- **1** — 560 µs mark, 560 µs space

## Building

This is a Flipper Application Package (FAP). To build it, clone it into your Flipper Zero firmware source tree:

```bash
# Clone the firmware (if you haven't already)
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware

# Clone this app into the external apps directory
git clone https://github.com/nmatt0/magiquest-wand.git applications_user/magiquest_wand

# Build the FAP
./fbt fap_magiquest_wand
```

The compiled `.fap` file will be in `build/f7-firmware-D/.extapps/magiquest_wand.fap`.

## Installing

### From source (after building)

```bash
# Deploy directly to a connected Flipper Zero via USB
./fbt launch app=magiquest_wand
```

### Manual install

Copy the compiled `magiquest_wand.fap` to your Flipper Zero's SD card at:

```
/ext/apps/Infrared/magiquest_wand.fap
```

Then launch it from **Apps → Infrared → MagiQuest Wand** on your Flipper.

## Usage

1. **Capture a wand** — Select "Capture Wand" from the main menu, then wave a MagiQuest wand at the Flipper's IR port. The app will decode the signal and show the wand details. Press "Save" to name and store it.
2. **Add manually** — Select "Add Wand" to type in a name and wand ID by hand.
3. **Transmit** — Open a saved wand and press "Send" to replay its signal.
4. **Edit** — Open a saved wand and press "Config" to modify the wand ID or magnitude.

## Project Structure

```
├── application.fam              # Flipper app manifest
├── magiquest_wand.h             # App struct, enums, constants
├── magiquest_wand.c             # App entry point and lifecycle
├── protocol/
│   ├── magiquest_protocol.h     # IR protocol constants and API
│   └── magiquest_protocol.c     # Signal encoder and decoder
├── storage/
│   ├── magiquest_storage.h      # Persistent storage API
│   └── magiquest_storage.c      # FlipperFormat read/write
└── scenes/
    ├── magiquest_scene.h        # Scene handler declarations
    ├── magiquest_scene.c        # Scene handler table
    ├── magiquest_scene_main_menu.c
    ├── magiquest_scene_wand_list.c
    ├── magiquest_scene_wand_add.c
    ├── magiquest_scene_wand_capture.c
    ├── magiquest_scene_wand_detail.c
    ├── magiquest_scene_config.c
    └── magiquest_scene_transmit.c
```

## License

MIT
