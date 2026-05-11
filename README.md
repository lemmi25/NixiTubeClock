# ESP32 NixiTube OTA Clock

<p align="center">
<img src="https://github.com/lemmi25/NixiTubeClock/blob/master/images/Main.JPG">
</p>

## Hardware Profiles

Firmware variants live in src/hardware:

- master_no_rtc.cpp
	- No RTC module
	- Time sync from internet API
	- FreeRTOS tasks (display/buttons and WiFi/OTA)
- rtc_local.cpp
	- DS1307 RTC module
	- Local time from RTC with optional internet sync
	- Power and menu button handling in single-loop runtime

## Tube Types

Tube mapping is selected in include/clock_variant_config.h via config.env:

- ZM1000
- IN4
- DA2000 (numitron)

## Main Configuration

All build-time options are read from config.env by extra_scripts/read_config_env.py.
Each key in config.env becomes a C/C++ define automatically.

Typical minimal setup:

```env
HARDWARE_PROFILE=RTC_LOCAL
TUBE_TYPE=ZM1000

ENABLE_OTA=1
OTA_IP=192.168.10.56
OTA_PASSWORD=nixiclock

WIFI_SSID=YOUR_WIFI_SSID
WIFI_PASSWORD=YOUR_WIFI_PASSWORD
CITY=Europe/Berlin
TIME_API_BASE=https://timeapi.io/api/Time/current/zone?timeZone=
```

## OTA Update

OTA is enabled with ENABLE_OTA=1.

- OTA stays always available after WiFi is connected.
- Flash command:

```bash
./scripts/ota_flash_monitor.sh
```

The script reads OTA_IP, OTA_PASSWORD, OTA_PORT from config.env, builds firmware, uploads over OTA, then opens serial monitor.

## Buttons And LEDs

Current runtime behavior:

- Power button (BTN_ON / GPIO3): toggle hibernate and wake
- Menu button (BTN_MODE / GPIO15): short/long press function handling
- LED brightness level: LED_DIM
	- master_no_rtc currently uses LED_DIM in firmware defaults
	- rtc_local supports LED_DIM from config.env and includes boot LED intro animation

## Build

USB build/upload:

```bash
platformio run -e esp-wrover-kit
```

OTA build/upload environment:

```bash
platformio run -e esp-wrover-kit-ota
```
