#pragma once

// GPS
#define GPS_UART UART_NUM_1  // UART for GPS
#define GPS_PinTx     43 // Tx-Data
#define GPS_PinRx     44 // Rx-Data
//#define GPS_PinPPS    0 // PPS

// SX1276 RF chip
#define Radio_PinRST  1 //
#define Radio_PinSCK  14 // SCK
#define Radio_PinMOSI 13 // MOSI
#define Radio_PinMISO 21 // MISO
#define Radio_PinCS   46 // CS
#define Radio_PinIRQ  10 // IRQ
// additional pins for SX1262
#define Radio_PinIRQ1 18 // IRQ
#define Radio_PinBusy 47 // Busy

// I2C
#define I2C_PinSCL    40 // SCL
#define I2C_PinSDA    39 // SDA

