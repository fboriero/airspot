#include <TinyGPS++.h>
#include "ExtensionIOXL9555.hpp"

bool gps_init();
bool setupGPS();
static int getAck(uint8_t *buffer, uint16_t size, uint8_t requestedClass, uint8_t requestedID);
static bool GPS_Recovery();

