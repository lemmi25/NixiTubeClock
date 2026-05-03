---
name: debug-clock-hardware
description: 'Debug all NixiTubeClock hardware variants and modules: no-RTC and RTC profiles, tube mappings (ZM1000/IN4/DA2000), WiFi provisioning AP, shift-register output, RTC wiring, and display behavior.'
argument-hint: 'Optional: describe symptom, e.g. "setup AP not visible", "digits swapped", "RTC loses time", or "wrong tube output"'
---

# Debug Clock Hardware - NixiTubeClock

## When to Use
- Setup hotspot is not visible or web portal does not open
- WiFi credentials are saved but internet sync still fails
- RTC profile does not keep time between sync cycles
- No-RTC profile drifts or stops updating time
- Digits appear swapped or incorrect for your tube type
- DA2000 numitron output is wrong while nixie tubes work
- Display is blank or only partial digits are lit

## Hardware Profiles

Configured in `config.env`:

| Key | Values | Behavior |
|---|---|---|
| `HARDWARE_PROFILE` | `MASTER_NO_RTC`, `RTC_LOCAL` | Selects no-RTC or RTC firmware path |
| `TUBE_TYPE` | `ZM1000`, `IN4`, `DA2000` | Selects segment mapping and numitron mode |

Relevant files:
- [src/hardware/master_no_rtc.cpp](../../../src/hardware/master_no_rtc.cpp)
- [src/hardware/rtc_local.cpp](../../../src/hardware/rtc_local.cpp)
- [include/clock_variant_config.h](../../../include/clock_variant_config.h)
- [src/nixiDriver.cpp](../../../src/nixiDriver.cpp)

## Provisioning AP and Web Portal Checks

Provisioning implementation:
- [src/hardware/clock_provisioning.cpp](../../../src/hardware/clock_provisioning.cpp)
- [include/clock_provisioning.h](../../../include/clock_provisioning.h)

Expected behavior:
1. If config is missing/invalid, ESP opens AP (`PROVISION_AP_NAME`)
2. Connect to AP and open `http://192.168.4.1`
3. Save `ssid`, `password`, `city`
4. Device restarts and uses saved settings

If AP is not visible:
- Verify `PROVISION_AP_NAME` in `config.env`
- Ensure placeholders are not left as real credentials
- Check monitor output for `Entering provisioning mode` and `AP IP`

## RTC Path Diagnostics (`RTC_LOCAL`)

- Confirm DS1307 wiring and I2C lines
- Confirm backup battery polarity and charge
- Verify `rtc.begin()` and `rtc.isrunning()` path at boot
- If internet sync fails repeatedly, device should re-enter provisioning mode

## No-RTC Path Diagnostics (`MASTER_NO_RTC`)

- Device syncs internet time every `WIFI_SYNC_SECONDS` (default 3600)
- Between syncs, display time uses internal millis-based counter
- If sync fails `MAX_SYNC_FAILS` times, provisioning portal should open

## Tube Mapping Diagnostics

Mappings live in:
- [include/clock_variant_config.h](../../../include/clock_variant_config.h)

Quick checks:
- `ZM1000`: straight mapping 1-2-3-4
- `IN4`: swapped first two segments
- `DA2000`: reverse segment ordering and numitron logic

If digits are wrong:
1. Confirm `TUBE_TYPE` in `config.env`
2. Rebuild and verify build output shows selected type
3. If still wrong, check wiring from shift register outputs to tube drivers

## Build/Config Validation Checklist

1. Confirm `config.env` exists and is parsed
2. Confirm `HARDWARE_PROFILE` and `TUBE_TYPE` are valid
3. Confirm pre-script is enabled in `platformio.ini`
4. Rebuild after config changes (`clean` + build)

## Key Files
- [config.env.example](../../../config.env.example)
- [config.env](../../../config.env)
- [platformio.ini](../../../platformio.ini)
- [extra_scripts/read_config_env.py](../../../extra_scripts/read_config_env.py)
- [src/hardware/hardware_profile_guard.cpp](../../../src/hardware/hardware_profile_guard.cpp)
