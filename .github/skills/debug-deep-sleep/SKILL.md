---
name: debug-deep-sleep
description: 'Debug hardware profile and time-source issues on NixiTubeClock. Use when wrong firmware path is active (RTC vs no RTC), tube mapping is incorrect, time is not updating, or config.env values are ignored.'
argument-hint: 'Optional: describe the symptom, e.g. "RTC board still uses internet" or "digits are mapped wrong"'
---

# Debug Hardware Profile & Time Source — NixiTubeClock

## When to Use
- Device never wakes from sleep (or wakes too frequently)
- RTC board unexpectedly trying to pull internet time
- No-RTC board not updating time from internet
- Digits shown in wrong order for selected tube type
- Build appears to ignore values from config.env

## Config Architecture

Build-time configuration is loaded from `config.env` by:

- [extra_scripts/read_config_env.py](../../../extra_scripts/read_config_env.py)
- [platformio.ini](../../../platformio.ini)

Core selectors:

| Key | Valid values | Effect |
|---|---|---|
| `HARDWARE_PROFILE` | `MASTER_NO_RTC`, `RTC_LOCAL` | Enables exactly one hardware source file |
| `TUBE_TYPE` | `ZM1000`, `IN4`, `DA2000` | Selects segment mapping and numitron mode |

## Hardware Paths

- `MASTER_NO_RTC` -> [src/hardware/master_no_rtc.cpp](../../../src/hardware/master_no_rtc.cpp)
  Uses WiFi + HTTP time endpoint.
- `RTC_LOCAL` -> [src/hardware/rtc_local.cpp](../../../src/hardware/rtc_local.cpp)
  Uses DS1307 RTC and no internet time sync.

If neither or both profiles become active, build fails in:
- [src/hardware/hardware_profile_guard.cpp](../../../src/hardware/hardware_profile_guard.cpp)

## Tube Mapping Checks

Mapping comes from [include/clock_variant_config.h](../../../include/clock_variant_config.h).
If digits are swapped or mirrored, confirm `TUBE_TYPE` in `config.env` matches hardware.

## Diagnostic Checklist

1. Verify `config.env` exists in project root
2. Confirm `HARDWARE_PROFILE` is one of: `MASTER_NO_RTC`, `RTC_LOCAL`
3. Confirm `TUBE_TYPE` is one of: `ZM1000`, `IN4`, `DA2000`
4. Rebuild and check output line from script:
   `config.env loaded: ... HARDWARE_PROFILE=... TUBE_TYPE=...`
5. If on no-RTC profile, verify `WIFI_SSID`, `WIFI_PASSWORD`, and `TIME_API_URL`
6. If on RTC profile, confirm RTC wiring and battery state

## Common Failure Signatures

| Symptom | Likely cause | Fix |
|---|---|---|
| Build error from hardware profile guard | Invalid or missing `HARDWARE_PROFILE` | Set valid value in `config.env` |
| No-RTC board shows blanks | WiFi/time API unavailable | Check credentials and endpoint |
| RTC board drifts or resets time | RTC not running or lost backup | Recheck RTC battery/wiring |
| Wrong digit order | Wrong tube type selected | Set correct `TUBE_TYPE` |

## Key Files
- [platformio.ini](../../../platformio.ini) — build environment and extra script hook
- [config.env.example](../../../config.env.example) — expected config format
- [extra_scripts/read_config_env.py](../../../extra_scripts/read_config_env.py) — parser and define injection
- [src/hardware/master_no_rtc.cpp](../../../src/hardware/master_no_rtc.cpp) — internet time profile
- [src/hardware/rtc_local.cpp](../../../src/hardware/rtc_local.cpp) — RTC time profile
