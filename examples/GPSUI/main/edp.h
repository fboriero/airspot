
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <esp_sleep.h>
#include <esp_timer.h>
#include <esp_types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <epdiy.h>

#include "sdkconfig.h"
#include "firasans_12.h"
#include "firasans_20.h"

// Arduino
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>


#define WAVEFORM EPD_BUILTIN_WAVEFORM

// choose the default demo board depending on the architecture
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define DEMO_BOARD epd_board_v7
#endif

void idf_setup();
static inline void checkError(enum EpdDrawError err);
void showGpsInfo(double infoLat, double infoLng);