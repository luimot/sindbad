#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <mbed.h>
#include "BufferedSerial.h"

#include "PinNames.h"
#include "Data.h"

#include "TeleRadio.h"
#include "TelePositioning.h"

Data data;
DigitalOut debugLED(LED1);
static BufferedSerial port(USBTX,USBRX,115200);
FileHandle *mbed::mbed_override_console(int fd) { return &port; }
void setupUSBSerial()
{
    port.set_format(8);
}

int main(){
    setupUSBSerial();
    debugPrint("Start program\n");
    TeleRadio tradio;
    TelePositioning tposition;
    tposition.init();
    tradio.init();
    PositionInfo pos;
    // tposition.test();
    while(1){
        tposition.updatePositionInfo(&pos);
        // DEBUG ONLY, REPLACE WITH A DEBUG LOG THAT _CANNOT_ GO INTO LAUNCH OTHERWISE CODE WILL BE SLOW
        printf("\tLatitude: %.5f\n\tLongitude: %.5f\n",pos.position.latitude,pos.position.longitude);
        printf("\tElevation: %.1f\n\t Speed: %.1f\n",pos.position.elevation,pos.position.speed);
        int time = pos.satelliteInfo.satTime;
        int hour, min, sec;
        hour = time / 10000;
        time %= 10000;
        min = time / 100;
        sec = time % 100;
        printf("\tUTC Time: %02d h %02d min %02d s\n",hour,min,sec);
        int date = pos.satelliteInfo.satDate;
        int day, mon, yr;
        day = date / 10000;
        date %= 10000;
        mon = date / 100;
        yr = date % 100;
        printf("\tUTC Date: %02d / %02d / %d\n",day,mon,yr);
        // DEBUG ONLY, REPLACE WITH A DEBUG LOG THAT _CANNOT_ GO INTO LAUNCH OTHERWISE CODE WILL BE SLOW

        // if(tradio.radioStatus() != RF_TX_RUNNING){
        //     debugLED = 1;
        //     tradio.send(&data);
        // }
        // else{
        //     debugLED = 0;
        // }
    }
    
    return 0;
}