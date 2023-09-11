#include "BufferedSerial.h"
#include "mbed.h"
#include "TelePositioning.h"
#include <cstdint>
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
void TelePositioning::gnssParse(char* buffer, uint8_t length, PositionInfo* position){
    //Check if it's one of either: Galileo, Beidou, Glonass, GNSS or Combined
    if ((buffer[0] == '$') || buffer[1] == 'G'){
        if (_CHECK_TALKER(buffer,"GLL")){
            char ch;
            if (GnssSerial::getNmeaAngle(1, buffer, length, position->position.latitude) &&
                GnssSerial::getNmeaAngle(3, buffer, length, position->position.longitude) &&
                GnssSerial::getNmeaItem(6, buffer, length, ch) &&
                ch == 'A')
                debugPrint("GNSS: location %.5f %.5f %c.\n",
                            position->position.latitude, position->position.longitude, ch);
        }
        else if (_CHECK_TALKER(buffer,"GGA") || _CHECK_TALKER(buffer,"GNS"))
        {
            const char *pTimeString = NULL;
            int gpsFix;

            // Retrieve the time
            pTimeString = GnssSerial::findNmeaItemPos(1, buffer, buffer + length);
            // Retrieve gps Fix
            if(GnssSerial::getNmeaItem(6, buffer, length, gpsFix, 10)){
                position->satelliteInfo.gpsFix = gpsFix;
                debugPrint("GNSS: gpsFix is %d.\n",gpsFix);
            }
            // Retrieve numbSat
            int numbSat;
            if(GnssSerial::getNmeaItem(7,buffer,length,numbSat,10)){
                position->satelliteInfo.numbSat = (uint8_t)numbSat;
                debugPrint("GNSS: numbSat is %d.\n",gpsFix);
            }

            if (pTimeString != NULL)
            {
                position->satelliteInfo.satTime = atoi(pTimeString);
                debugPrint("GNSS: time is %.6s.\n", pTimeString);
            }

            if (GnssSerial::getNmeaItem(9, buffer, length, position->position.elevation)) // altitude msl [m]
            {
                debugPrint("GNSS: elevation: %.1f.\n", position->position.elevation);
            }
        }
        else if (_CHECK_TALKER(buffer,"VTG"))
        {
            if (GnssSerial::getNmeaItem(7, buffer, length, position->position.speed)) // speed [km/h]
            {
                debugPrint("GNSS: speed: %.1f.\n", position->position.speed);
            }
        }
        else if(_CHECK_TALKER(buffer, "RMC"))
        {
            if (GnssSerial::getNmeaItem(9,buffer,length,position->satelliteInfo.satDate,10)){
                debugPrint("GNSS: date: %d\n",position->satelliteInfo.satDate);
            }
        }
    }
}

void TelePositioning::gnssParse(char* buffer, uint8_t length, LatLong* position){
    //Check if it's one of either: Galileo, Beidou, Glonass, GNSS or Combined
    if ((buffer[0] == '$') || buffer[1] == 'G'){
        if (_CHECK_TALKER(buffer,"GLL")){
            char ch;
            if (GnssSerial::getNmeaAngle(1, buffer, length, position->latitude) &&
                GnssSerial::getNmeaAngle(3, buffer, length, position->longitude) &&
                GnssSerial::getNmeaItem(6, buffer, length, ch) &&
                ch == 'A')
                debugPrint("GNSS: location %.5f %.5f %c.\n",
                            position->latitude, position->longitude, ch);
        }
    }
}

void TelePositioning::updatePositionInfo(PositionInfo* posInfo){
    char buffer[BUFFERSIZE];
    int size = getDataFromGPS(buffer);
    gnssParse(buffer, size, posInfo);
}

LatLong TelePositioning::updateLatLong(){
    LatLong pos;
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
            // TODO: Make and test a ending verification, for instance, if
            // the message does not end with \n it is probably broke and index
            // can be returned as 0, checksum could also be a good thing
            // Indication of error: Incorrect date at $GNRMC
            break;
        }
        if(startNMEA && size > 0) message[index] = buffer[0];
        if(startNMEA == false && buffer[0] == '$'){
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
