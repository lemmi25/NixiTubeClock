#ifndef CLOCK_VARIANT_CONFIG_H
#define CLOCK_VARIANT_CONFIG_H

#include <Arduino.h>

// Tube type is selected via build flag from .env/platformio.ini.
#if defined(TUBE_TYPE_DA2000)
static const uint8_t SEGMENT_1 = 4;
static const uint8_t SEGMENT_2 = 3;
static const uint8_t SEGMENT_3 = 2;
static const uint8_t SEGMENT_4 = 1;
static const bool CLOCK_IS_NUMITRON = true;
#elif defined(TUBE_TYPE_IN4)
static const uint8_t SEGMENT_1 = 3;
static const uint8_t SEGMENT_2 = 4;
static const uint8_t SEGMENT_3 = 2;
static const uint8_t SEGMENT_4 = 1;
static const bool CLOCK_IS_NUMITRON = false;
#else
// Default: ZM1000
static const uint8_t SEGMENT_1 = 1;
static const uint8_t SEGMENT_2 = 2;
static const uint8_t SEGMENT_3 = 3;
static const uint8_t SEGMENT_4 = 4;
static const bool CLOCK_IS_NUMITRON = false;
#endif

#endif