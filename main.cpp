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
        pos = tposition.updatePositionInfo();
        printf("\tLatitude: %.5f\n\tLongitude: %.5f\n",pos.position.latitude,pos.position.longitude);
        printf("\tElevation: %.1f\n\t Speed: %.1f\n",pos.position.elevation,pos.position.speed);
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