#include "Data.h"
#include "TelePositioning.h"
#include "Storage.h"
#include "globalDefines.h"

class TeleData{
    private:
        void _updateGPS(Data* data);
        void _updateSensors();

    public:
        Storage storage;
        TelePositioning position;
        void storeData(Data data);
        int initSensors();
        void initializeData(Data* data);
        void updateData(Data* data);
        void debugPrintData(Data data);
};