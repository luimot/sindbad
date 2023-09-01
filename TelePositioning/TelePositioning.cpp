#include "TelePositioning.h"

void TelePositioning::init(){
    debugPrint("Starting GNSS...\n");
#ifdef GPS_UART
    gnss = new GnssSerial(GPS_TX,GPS_RX,GPS_BAUDRATE);
    debugPrint("UART GNSS Started...\n");
#else
    gnss = new GnssI2C(GPS_SDA,GPS_SCL);
#endif
    gnss->init();
}

// This function is heavily based on the ublox-gnss unit test `test_serial_time()` 
void TelePositioning::gnssParse(char* buffer, int returnCode, LatLong* pos){
#define _CHECK_TALKER(s) ((buffer[3] == s[0]) && (buffer[4] == s[1]) && (buffer[5] == s[2]))
    int32_t length = LENGTH(returnCode);
    if ((PROTOCOL(returnCode) == GnssParser::NMEA) && (length > 6)){
        //Check if it's one of either: Galileo, Beidou, Glonass, GNSS or Combined
        if ((buffer[0] == '$') || buffer[1] == 'G'){
            if (_CHECK_TALKER("GLL")){
                char ch;
                if (gnss->getNmeaAngle(1, buffer, length, pos->latitude) &&
                    gnss->getNmeaAngle(3, buffer, length, pos->longitude) &&
                    gnss->getNmeaItem(6, buffer, length, ch) &&
                    ch == 'A')
                {
                    pos->latitude *= 60000;
                    pos->longitude *= 60000;
                    debugPrint("GNSS: location %.5f %.5f %c.\n", pos->latitude, pos->longitude, ch);
                }
            }
            else if (_CHECK_TALKER("GGA") || _CHECK_TALKER("GNS"))
            {
                const char *pTimeString = NULL;

                // Retrieve the time
                pTimeString = gnss->findNmeaItemPos(1, buffer, buffer + length);
                if (pTimeString != NULL)
                {
                    debugPrint("GNSS: time is %.6s.\n", pTimeString);
                }

                if (gnss->getNmeaItem(9, buffer, length, pos->elevation)) // altitude msl [m]
                {
                    debugPrint("GNSS: elevation: %.1f.\n", pos->elevation);
                }
            }
            else if (_CHECK_TALKER("VTG"))
            {
                if (gnss->getNmeaItem(7, buffer, length, pos->speed)) // speed [km/h]
                {
                    debugPrint("GNSS: speed: %.1f.\n", pos->speed);
                }
            }
        }

    }


}

LatLong TelePositioning::updateLatLong(){
    LatLong pos;
    //TODO
    char buffer[256];
    int returnCode;
    memset(buffer,0,sizeof(buffer));
    // If getMessage returns something < 0 it means WAIT
    if(gnss->getMessage(buffer,sizeof(buffer)) != -1)
        debugPrint("%s\n",buffer); 
    // while((returnCode = gnss->getMessage(buffer,sizeof(buffer))) > 0){
    //     debugPrint("%s\n",buffer);
    //     gnssParse(buffer, returnCode, &pos);
    // }
    return pos;
}