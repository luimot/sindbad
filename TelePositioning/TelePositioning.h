#include "gnss.h"
#include "globalDefines.h"



typedef struct{
    double latitude;
    double longitude;
    double elevation;
    double speed;
} LatLong;

class TelePositioning{
    private:
#ifdef GPS_UART
    GnssSerial* gnss;
#else
    GnssI2C* gnss;
#endif
    /* GPS PRIVATE INTERACTION METHODS */
    void gnssParse(char* buffer, int returnCode, LatLong* pos);
    
    public:
    LatLong position;
    double gpsSpeed;

    /* GPS INTERACTION METHODS */
    void init();
    LatLong updateLatLong();
};

