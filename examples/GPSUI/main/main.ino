#include <esp32-hal.h>
#include <HardwareSerial.h>
#include "gps.h"
#include "edp.h"
#define SerialMon Serial

void setup(void)
{
    Serial.begin(115200);
    //Init Screen
    if (psramInit()) {
        Serial.println("\nThe PSRAM is correctly initialized");
    } else {
        Serial.println("\nPSRAM does not work");
    }

    idf_setup();
    gps_init();
}

void loop(void)
{
    Serial.println("\n .");
    delay(2000);
}

