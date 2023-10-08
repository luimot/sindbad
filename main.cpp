#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <mbed.h>
#include "BufferedSerial.h"

#include "PinNames.h"
#include "Data.h"

#include "TeleRadio.h"
#include "TeleData.h"

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
    // TeleRadio tradio;
    // TeleData tdata;
    // tradio.init();
    // tdata.initSensors();
    Storage st;
    st.initSDStorage();
    st.createSDFile();
    // st.initFlashStorage();
    // st.createFlashFile();
    // st.closeFlashFile();
    while(1){
        // tdata.updateData(&data);
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