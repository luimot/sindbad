#include "TeleData.h"

int TeleData::initSensors(){
    int error=0;
    // Starts GPS
    position.init();
    // TODO: Start sensors
    //Starts storage
    storage.initFlashStorage(); // Flash
    storage.createFlashFile();
    // TODO: Start SD Card
    return error;
}

void TeleData::_updateGPS(Data* data){
    PositionInfo posInfo;
    position.updatePositionInfo(&posInfo);

    data->latitude = (float)posInfo.position.latitude;
    data->longitude = (float)posInfo.position.longitude;
    data->elevation = (float)posInfo.position.elevation;
    data->speed = (float)posInfo.position.speed;

    data->numbSat = posInfo.satelliteInfo.numbSat;
    data->UTCDate = posInfo.satelliteInfo.satDate;
    data->UTCTime = posInfo.satelliteInfo.satTime;
}

void TeleData::updateData(Data* data){

    _updateGPS(data);
    // TODO: update Sensors
    debugPrintData(*data);
}

void TeleData::storeData(Data data){
    #ifdef FLASH_EN
    storage.writeFlashFile(data);
    #endif
    #ifdef SD_EN
    // TODO: Store SD card data
    #endif
}

void TeleData::debugPrintData(Data data){
    // Use this function with care, as it can heavily impact performance
    // See the DEBUG_MESSAGES define in `globalDefines.h`
    debugPrint("Data Sent:\n");
    debugPrint("Sensor Data:\n");
    debugPrint("\tPressure: %f\n",data.pressure);
    debugPrint("\tAcceleration(X,Y,Z): (%d,%d,%d)\n",data.a[0],data.a[1],data.a[2]);
    debugPrint("\t");   //TODO: Finish data print
    debugPrint("\tLatitude: %.5f\n\tLongitude: %.5f\n",data.latitude,data.longitude);
    debugPrint("\tElevation: %.1f\n\t Speed: %.1f\n",data.elevation,data.speed);
    debugPrint("\tGPS Fix: %d\n\t# of Satellites: %d\n",data.gpsFix,data.numbSat);
    #ifdef DEBUG_MESSAGES
    int time = data.UTCTime;
    int hour, min, sec;
    hour = time / 10000;
    time %= 10000;
    min = time / 100;
    sec = time % 100;
    #endif
    debugPrint("\tUTC Time: %02d h %02d min %02d s\n",hour,min,sec);
    #ifdef DEBUG_MESSAGES
    int date = data.UTCDate;
    int day, mon, yr;
    day = date / 10000;
    date %= 10000;
    mon = date / 100;
    yr = date % 100;
    #endif
    debugPrint("\tUTC Date: %02d / %02d / %d\n",day,mon,yr);
}