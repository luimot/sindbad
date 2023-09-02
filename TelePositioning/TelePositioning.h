#include "BufferedSerial.h"
#include "gnss.h"
#include "globalDefines.h"
#include <cstdint>

#define BUFFERSIZE 256

typedef struct{
    double latitude;
    double longitude;
    double elevation;
    double speed;
} LatLong;

typedef struct{
    uint8_t numbSat;
    uint8_t gpsFix;
    long satTime;
} SatInfo;

class TelePositioning{
    private:
#ifdef GPS_UART
    GnssSerial* parser;
    BufferedSerial*gnss;
#else
    GnssI2C* gnss;
#endif
    /* GPS PRIVATE INTERACTION METHODS */
    int getDataFromGPS(char* message);
    void gnssParse(char* buffer, int returnCode, LatLong* pos);
    void test_serial_time();
    
    public:
    LatLong position;

    /* GPS INTERACTION METHODS */
    void init();
    LatLong updateLatLong();
    void test();
};

