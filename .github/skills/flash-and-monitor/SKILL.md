---
name: flash-and-monitor
description: 'Build, upload (flash), and monitor NixiTubeClock firmware via PlatformIO. Use when flashing firmware, selecting hardware profile or tube type from config.env, opening serial monitor, and debugging over USB.'
argument-hint: 'Optional: port override, e.g. --upload-port /dev/ttyUSB0'
---

# Flash & Monitor — NixiTubeClock

## When to Use
- Uploading new firmware to the ESP32 clock board
- Opening the serial monitor to watch boot logs or debug output
- Verifying a build before or after flashing
- Verifying the selected hardware profile and tube mapping from config.env

## Config First

Before build/flash, edit `config.env` (create from `config.env.example`):

```env
HARDWARE_PROFILE=MASTER_NO_RTC
TUBE_TYPE=ZM1000
PROVISION_AP_NAME=NixiClockSetup
WIFI_SYNC_SECONDS=3600
MAX_SYNC_FAILS=3
TIME_API_BASE=http://worldtimeapi.org/api/timezone
WIFI_SSID=YOUR_WIFI_SSID
WIFI_PASSWORD=YOUR_WIFI_PASSWORD
CITY=Europe/Berlin
```

**Note:** `TIME_API_BASE` uses a global base URL (no continent prefix). Your `CITY` setting must use "Continent/City" format like "Europe/Berlin", "America/New_York", or "Asia/Tokyo".

## Prerequisites
- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) installed (or use the PlatformIO IDE extension in VS Code)
- ESP32 board connected via USB
- Working directory: project root (where `platformio.ini` lives)

## Manual Boot Mode (Custom PCB)

This PCB has **no auto-reset circuit** (no DTR/RTS → BOOT/EN wiring).  
You must put the ESP32 into download mode manually before every flash:

1. Hold down the **BOOT** button
2. Press and release the **RST** button *(keep BOOT held)*
3. Release the **BOOT** button
4. ESP32 is now in download mode — run the upload command within a few seconds

> `platformio.ini` already has `--before no_reset` so esptool will not try to auto-reset.  
> The flash script (`scripts/flash_and_monitor.sh`) will prompt you to complete these steps before uploading.

## Commands

### 1. Build only
```sh
pio run -e esp-wrover-kit
```

### 2. Build + Upload (flash)
```sh
pio run -e esp-wrover-kit -t upload
```

### 3. Serial monitor (57600 baud, matches `monitor_speed` in platformio.ini)
```sh
pio device monitor --baud 115200
```

### 4. Build, upload, then immediately open serial monitor
```sh
pio run -e esp-wrover-kit -t upload && pio device monitor --baud 115200
```

## Port Troubleshooting

List all connected serial devices:
```sh
pio device list
```

Override the upload port manually:
```sh
pio run -e esp-wrover-kit -t upload --upload-port /dev/ttyUSB0
```

On Linux, if you get a **permission denied** error on the serial port:
```sh
sudo usermod -aG dialout $USER   # then log out and back in
```

## Expected Behavior by Hardware Profile

**MASTER_NO_RTC**
- Connects to WiFi and fetches time from `TIME_API_BASE` + `CITY`
- Displays hour/minute from internet response

**RTC_LOCAL**
- Reads time from DS1307 RTC
- Does not require internet time sync

## Error Conditions

| Serial output | Meaning | What happens next |
|---|---|---|
| `config.env loaded ...` | Build script parsed config values | Confirms selected hardware and tube |
| `Unknown HARDWARE_PROFILE=...` | Invalid profile value in config.env | Build uses no valid profile and fails at compile guard |
| `Unknown TUBE_TYPE=...` | Invalid tube selection in config.env | Build has no matching tube mapping |

See the `debug-clock-hardware` skill for full hardware diagnostics.

## Key Files
- [platformio.ini](../../../platformio.ini) — build environment, script integration, source filter
- [config.env.example](../../../config.env.example) — template for local config
- [extra_scripts/read_config_env.py](../../../extra_scripts/read_config_env.py) — config.env parser
- [src/hardware/master_no_rtc.cpp](../../../src/hardware/master_no_rtc.cpp) — no RTC profile
- [src/hardware/rtc_local.cpp](../../../src/hardware/rtc_local.cpp) — RTC profile
