# ESP32 NixiTube OTA Clock

<p align="center"> 
<img src="https://github.com/lemmi25/NixiTubeClock/blob/master/images/Main.JPG">
</p>

**Please have a look around**

## Hardware Variants

Firmware is split by hardware version in `src/hardware/`:

- `master_no_rtc.cpp`: No RTC hardware, time comes from internet API.
- `rtc_local.cpp`: RTC hardware, time is read from RTC and not pulled from internet.

## Tube Variants

Tube mapping is shared in `include/clock_variant_config.h` and supports:

- `ZM1000`
- `IN4`
- `DA2000` (numitron)

## Common Selection File

Select both hardware and tube from `config.env`:

```env
HARDWARE_PROFILE=MASTER_NO_RTC
TUBE_TYPE=ZM1000
WIFI_SSID=YOUR_WIFI_SSID
WIFI_PASSWORD=YOUR_WIFI_PASSWORD
TIME_API_URL=http://worldtimeapi.org/api/timezone/Europe/Berlin.json
```

Use `config.env.example` as template and keep `config.env` local.

Then build normally with PlatformIO.
