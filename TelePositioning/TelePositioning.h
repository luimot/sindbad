#include "BufferedSerial.h"
#include "gnss.h"
#include "globalDefines.h"
#include <cstdint>

#define BUFFERSIZE 256

/* GPS AND PARSER FUNCTION-LIKE MACROS */
#define COUNT_TIME(t) std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count()
#define DIDNT_TIMEOUT(t) COUNT_TIME(t) < GPS_TIMEOUT
#define _CHECK_TALKER(buffer,s) ((buffer[3] == s[0]) && (buffer[4] == s[1]) && (buffer[5] == s[2]))


typedef struct{
    double latitude;
    double longitude;
    double elevation;
    double speed;
} LatLong;

typedef struct{
    uint8_t numbSat;
    uint8_t gpsFix;
    int satTime;
    int satDate;
} SatInfo;

typedef struct{
    LatLong position;
    SatInfo satelliteInfo;
} PositionInfo;

class TelePositioning{
    private:
#ifdef GPS_UART
    BufferedSerial*gnss;
#else
    GnssI2C* gnss;
#endif
    /* GPS PRIVATE INTERACTION METHODS */
    int getDataFromGPS(char* message);
    void gnssParse(char* buffer, int returnCode, PositionInfo* position);
    void gnssParse(char* buffer, int length, LatLong* position);
    
    public:
    LatLong position;
    SatInfo sattelite;
    /* GPS INTERACTION METHODS */
    void init();
    LatLong updateLatLong();
    void updatePositionInfo(PositionInfo* posInfo);
};

