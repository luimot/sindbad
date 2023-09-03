#include "BufferedSerial.h"
#include "mbed.h"
#include "TelePositioning.h"
#include <cstring>
void TelePositioning::init(){
    debugPrint("Starting GNSS...\n");
#ifdef GPS_UART
    gnss = new BufferedSerial(GPS_TX,GPS_RX);
    gnss->set_baud(GPS_BAUDRATE);
    gnss->set_format(8);
    debugPrint("UART GNSS Starting...\n");
#else
    gnss = new GnssI2C(GPS_SDA,GPS_SCL);
#endif
}

// This function is heavily based on the ublox-gnss unit test `test_serial_time()` if not an almost copy-paste
void TelePositioning::gnssParse(char* buffer, int length, PositionInfo* position){
    //Check if it's one of either: Galileo, Beidou, Glonass, GNSS or Combined
    if ((buffer[0] == '$') || buffer[1] == 'G'){
        if (_CHECK_TALKER(buffer,"GLL")){
            char ch;
            if (parser->getNmeaAngle(1, buffer, length, position->position.latitude) &&
                parser->getNmeaAngle(3, buffer, length, position->position.longitude) &&
                parser->getNmeaItem(6, buffer, length, ch) &&
                ch == 'A')
                debugPrint("GNSS: location %.5f %.5f %c.\n",
                            position->position.latitude, position->position.longitude, ch);
        }
        else if (_CHECK_TALKER(buffer,"GGA") || _CHECK_TALKER(buffer,"GNS"))
        {
            const char *pTimeString = NULL;
            int gpsFix;

            // Retrieve the time
            pTimeString = parser->findNmeaItemPos(1, buffer, buffer + length);
            // Retrieve gps Fix
            if(parser->getNmeaItem(6, buffer, length, gpsFix, 10)){
                position->satelliteInfo.gpsFix = gpsFix;
                debugPrint("GNSS: gpsFix is %d.\n",gpsFix);
            }
            // Retrieve numbSat
            //TODO

            if (pTimeString != NULL)
            {
                position->satelliteInfo.satTime = atoi(pTimeString);
                debugPrint("GNSS: time is %.6s.\n", pTimeString);
            }

            if (parser->getNmeaItem(9, buffer, length, position->position.elevation)) // altitude msl [m]
            {
                debugPrint("GNSS: elevation: %.1f.\n", position->position.elevation);
            }
        }
        else if (_CHECK_TALKER(buffer,"VTG"))
        {
            if (parser->getNmeaItem(7, buffer, length, position->position.speed)) // speed [km/h]
            {
                debugPrint("GNSS: speed: %.1f.\n", position->position.speed);
            }
        }
    }
}

void TelePositioning::gnssParse(char* buffer, int length, LatLong* position){
    //Check if it's one of either: Galileo, Beidou, Glonass, GNSS or Combined
    if ((buffer[0] == '$') || buffer[1] == 'G'){
        if (_CHECK_TALKER(buffer,"GLL")){
            char ch;
            if (parser->getNmeaAngle(1, buffer, length, position->latitude) &&
                parser->getNmeaAngle(3, buffer, length, position->longitude) &&
                parser->getNmeaItem(6, buffer, length, ch) &&
                ch == 'A')
                debugPrint("GNSS: location %.5f %.5f %c.\n",
                            position->latitude, position->longitude, ch);
        }
    }
}

PositionInfo TelePositioning::updatePositionInfo(){
    PositionInfo posInfo;
    char buffer[BUFFERSIZE];
    int size = getDataFromGPS(buffer);
    gnssParse(buffer, size, &posInfo);
    return posInfo; 
}

LatLong TelePositioning::updateLatLong(){
    LatLong pos;
    SatInfo info;
    char buffer[BUFFERSIZE];
    int size = getDataFromGPS(buffer);
    gnssParse(buffer, size, &pos);
    return pos;
}

int TelePositioning::getDataFromGPS(char* message){
    char buffer[1];
    bool startNMEA = false;
    int size;
    int index = 0;
    Timer timeoutTimer;
    timeoutTimer.start();
    while(DIDNT_TIMEOUT(timeoutTimer)){
        size=0;
        memset(buffer, 0, sizeof(buffer));
        if(gnss->readable()){
            size = gnss->read(buffer, sizeof(buffer)); //Read data
        }

        // Copy message from buffer
        if(buffer[0] == '\n'){
            startNMEA = false;
            message[index] = buffer[0];
            // index = 0;
            break;
        }
        if(startNMEA && size > 0) message[index] = buffer[0];
        if(startNMEA == false && strcmp(buffer, "$")){
            index = 0;
            memset(message,0,BUFFERSIZE);
            message[0] = buffer[0];
            startNMEA = true;
        }
        if(startNMEA && size > 0) index++;
        if(index >= 255) index = 0; // Safety
    }
    debugPrint("Time: %lld ms\n",COUNT_TIME(timeoutTimer));
    timeoutTimer.stop();
    debugPrintNMEA("%s",message);
    return index;
}
