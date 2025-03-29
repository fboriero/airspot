#include "gps.h"
ExtensionIOXL9555 io;

#define SerialGPS Serial1
#define BOARD_GPS_RXD 44
#define BOARD_GPS_TXD 43
#define BOARD_GPS_PPS 1

#define SENSOR_SDA 39
#define SENSOR_SCL 40
#define SENSOR_IRQ -1

// The TinyGPSPlus object
TinyGPSPlus gps;
uint8_t buffer[256];

bool gps_init(void)
{
    const uint8_t chip_address = XL9555_SLAVE_ADDRESS0;
    // Init GPS
    if (!io.init(Wire, SENSOR_SDA, SENSOR_SCL, chip_address))
    {
        while (1)
        {
            Serial.println("Failed to find XL9555 - check your wiring!");
            delay(1000);
        }
    }

    // Set PORT0 as input,mask = 0xFF = all pin input
    io.configPort(ExtensionIOXL9555::PORT0, 0x00);
    // Set PORT1 as input,mask = 0xFF = all pin input
    io.configPort(ExtensionIOXL9555::PORT1, 0xFF);

    Serial.println("Power on LoRa and GPS!");
    io.digitalWrite(ExtensionIOXL9555::IO0, HIGH);
    gps_init();

    bool result = false;
    // L76K GPS USE 9600 BAUDRATE
    result = setupGPS();
    if (!result)
    {
        // Set u-blox m10q gps baudrate 38400
        SerialGPS.begin(38400, SERIAL_8N1, BOARD_GPS_RXD, BOARD_GPS_TXD);
        result = GPS_Recovery();
        if (!result)
        {
            SerialGPS.updateBaudRate(9600);
            result = GPS_Recovery();
            if (!result)
            {
                Serial.println("GPS Connect failed~!");
                result = false;
            }
            SerialGPS.updateBaudRate(38400);
        }
    }
    return result;
}


//Service

bool setupGPS()
{
    // L76K GPS USE 9600 BAUDRATE
    SerialGPS.begin(9600, SERIAL_8N1, BOARD_GPS_RXD, BOARD_GPS_TXD);
    bool result = false;
    uint32_t startTimeout;

    // Initialize the L76K Chip, use GPS + GLONASS
    SerialGPS.write("$PCAS04,5*1C\r\n");
    delay(250);
    SerialGPS.write("$PCAS03,1,1,1,1,1,1,1,1,1,1,,,0,0*02\r\n");
    delay(250);
    // Switch to Vehicle Mode, since SoftRF enables Aviation < 2g
    SerialGPS.write("$PCAS11,3*1E\r\n");
    Serial.println("Initialize the L76K Chip");
    return result;
}

static int getAck(uint8_t *buffer, uint16_t size, uint8_t requestedClass, uint8_t requestedID)
{
    uint16_t ubxFrameCounter = 0;
    bool ubxFrame = 0;
    uint32_t startTime = millis();
    uint16_t needRead;

    while (millis() - startTime < 800)
    {
        while (SerialGPS.available())
        {
            int c = SerialGPS.read();
            switch (ubxFrameCounter)
            {
            case 0:
                if (c == 0xB5)
                {
                    ubxFrameCounter++;
                }
                break;
            case 1:
                if (c == 0x62)
                {
                    ubxFrameCounter++;
                }
                else
                {
                    ubxFrameCounter = 0;
                }
                break;
            case 2:
                if (c == requestedClass)
                {
                    ubxFrameCounter++;
                }
                else
                {
                    ubxFrameCounter = 0;
                }
                break;
            case 3:
                if (c == requestedID)
                {
                    ubxFrameCounter++;
                }
                else
                {
                    ubxFrameCounter = 0;
                }
                break;
            case 4:
                needRead = c;
                ubxFrameCounter++;
                break;
            case 5:
                needRead |= (c << 8);
                ubxFrameCounter++;
                break;
            case 6:
                if (needRead >= size)
                {
                    ubxFrameCounter = 0;
                    break;
                }
                if (SerialGPS.readBytes(buffer, needRead) != needRead)
                {
                    ubxFrameCounter = 0;
                }
                else
                {
                    return needRead;
                }
                break;

            default:
                break;
            }
        }
    }
    return 0;
}

static bool GPS_Recovery()
{
    uint8_t cfg_clear1[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1C, 0xA2};
    uint8_t cfg_clear2[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xA1};
    uint8_t cfg_clear3[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x1D, 0xB3};
    SerialGPS.write(cfg_clear1, sizeof(cfg_clear1));

    if (getAck(buffer, 256, 0x05, 0x01))
    {
        Serial.println("Get ack successes!");
    }
    SerialGPS.write(cfg_clear2, sizeof(cfg_clear2));
    if (getAck(buffer, 256, 0x05, 0x01))
    {
        Serial.println("Get ack successes!");
    }
    SerialGPS.write(cfg_clear3, sizeof(cfg_clear3));
    if (getAck(buffer, 256, 0x05, 0x01))
    {
        Serial.println("Get ack successes!");
    }

    // UBX-CFG-RATE, Size 8, 'Navigation/measurement rate settings'
    uint8_t cfg_rate[] = {0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};
    SerialGPS.write(cfg_rate, sizeof(cfg_rate));
    if (getAck(buffer, 256, 0x06, 0x08))
    {
        Serial.println("Get ack successes!");
    }
    else
    {
        return false;
    }
    return true;
}