#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <mbed.h>
#include "BufferedSerial.h"

#include "PinNames.h"
#include "ThisThread.h"
#include "Data.h"

#include "TeleRadio.h"

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
    TeleRadio tradio;
    tradio.init();
    debugLED = 1;    
    ThisThread::sleep_for(2s);
    debugLED = 0;
    ThisThread::sleep_for(2s);
    while(1){
        if(tradio.radioStatus() != RF_TX_RUNNING){
            debugLED = 1;
            tradio.send(&data);
        }
        else{
            debugLED = 0;
        }
    }
    
    return 0;
}