#include "BufferedSerial.h"
#include "mbed.h"
#include "TelePositioning.h"
#include <cstring>
void TelePositioning::init(){
    debugPrint("Starting GNSS...\n");
#ifdef GPS_UART
    gnss = new GnssSerial(GPS_TX,GPS_RX,GPS_BAUDRATE);
    debugPrint("UART GNSS Starting...\n");
#else
    gnss = new GnssI2C(GPS_SDA,GPS_SCL);
#endif
    bool status = gnss->init();
    status?debugPrint("Connected\n"):debugPrint("Not connected\n");
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
    char buffer[512];
    int returnCode;
    // If getMessage returns something < 0 it means WAIT
    // BufferedSerial device(GPS_TX,GPS_RX);
    // device.set_baud(9600);
    // bool status = device.sync();
    // status?debugPrint("Connected\n"):debugPrint("Not connected\n");
    // test_serial_time()
    GnssSerial device(GPS_TX,GPS_RX,9600);
    bool status = device.init();
    status?debugPrint("Connected\n"):debugPrint("Not connected\n");
    while(1){
        memset(buffer,0,sizeof(buffer));
        int size = device.getMessage(buffer,sizeof(buffer));
        if(size>0) debugPrint("%s",buffer);
    }
    // while((returnCode = gnss->getMessage(buffer,sizeof(buffer))) > 0){
    //     debugPrint("%s\n",buffer);
    //     gnssParse(buffer, returnCode, &pos);
    // }
    return pos;
}

void TelePositioning::test_serial_time() {
    GnssSerial *pGnss = new GnssSerial(GPS_TX,GPS_RX);

    bool gotLatLong = false;
    bool gotElevation = false;
    bool gotSpeed = false;
    bool gotTime = false;
    char buffer[256];
    int returnCode;
    double latitude;
    double longitude;
    double elevation;
    double speed;

    printf("GNSS: powering up and waiting up to %d second(s) for something to happen.\n", 120);
    pGnss->init();

    memset(buffer, 0, sizeof(buffer));
    for (uint32_t x = 0; (x < 120) && !gotTime; x++)
    {
        while (((returnCode = pGnss->getMessage(buffer, sizeof(buffer))) > 0) &&
                !(gotLatLong && gotElevation && gotSpeed && gotTime))
        {
            int32_t length = LENGTH(returnCode);

            if ((PROTOCOL(returnCode) == GnssParser::NMEA) && (length > 6))
            {
                printf(".");

                // talker is $GA=Galileo $GB=Beidou $GL=Glonass $GN=Combined $GP=GNSS
                if ((buffer[0] == '$') || buffer[1] == 'G')
                {
#define _CHECK_TALKER(s) ((buffer[3] == s[0]) && (buffer[4] == s[1]) && (buffer[5] == s[2]))
                    if (_CHECK_TALKER("GLL"))
                    {
                        char ch;

                        if (pGnss->getNmeaAngle(1, buffer, length, latitude) &&
                            pGnss->getNmeaAngle(3, buffer, length, longitude) &&
                            pGnss->getNmeaItem(6, buffer, length, ch) &&
                            ch == 'A')
                        {
                            gotLatLong = true;
                            latitude *= 60000;
                            longitude *= 60000;
                            printf("\nGNSS: location %.5f %.5f %c.\n", latitude, longitude, ch);
                        }
                    }
                    else if (_CHECK_TALKER("GGA") || _CHECK_TALKER("GNS"))
                    {
                        const char *pTimeString = NULL;

                        // Retrieve the time
                        pTimeString = pGnss->findNmeaItemPos(1, buffer, buffer + length);
                        if (pTimeString != NULL)
                        {
                            gotTime = true;
                            printf("\nGNSS: time is %.6s.", pTimeString);
                        }

                        if (pGnss->getNmeaItem(9, buffer, length, elevation)) // altitude msl [m]
                        {
                            gotElevation = true;
                            printf("\nGNSS: elevation: %.1f.", elevation);
                        }
                    }
                    else if (_CHECK_TALKER("VTG"))
                    {
                        if (pGnss->getNmeaItem(7, buffer, length, speed)) // speed [km/h]
                        {
                            gotSpeed = true;
                            printf("\nGNSS: speed: %.1f.", speed);
                        }
                    }
                }
            }
        }
        // Altered for sindbad project Mbed6, uses wait_us()
        wait_us(1000e3);
    }

    printf("\n");

    // Depending on antenna positioning we may not be able to get a GNSS fix but we
    // should at least be able to receive the time from a satellite
}




void TelePositioning::test(){
    char buffer[1];
    char message[256];
    bool startNMEA = false;
    int size;
    int index = 0;
    BufferedSerial* p;
    p = new BufferedSerial(GPS_TX,GPS_RX);
    p->set_baud(9600);
    p->set_format(8);
    while(1){
        size=0;
        memset(buffer, 0, sizeof(buffer));
        if(p->readable())
            size = p->read(buffer, sizeof(buffer)); //Read data

        // Copy message from buffer
        if(strcmp(buffer, "\n") == 0){
            startNMEA = false;
            message[index] = buffer[0];
            index = 0;
        }
        if(startNMEA && size > 0) message[index] = buffer[0];
        if(startNMEA == false && strcmp(buffer, "$")){
            debugPrint("%s\n",message);
            index = 0;
            memset(message,0,sizeof(message));
            message[0] = buffer[0];
            startNMEA = true;
        }
        if(startNMEA && size > 0) index++;
        if(index >= 255) index = 0; // Safety
        // if(size > 0) debugPrint("%s\n",buffer);
    }
}