#include "Data.h"
#include "TelePositioning.h"
#include "globalDefines.h"

class TeleData{
    private:
        void _updateGPS(Data* data);
        void _updateSensors();

    public:
        TelePositioning position;
        int initSensors();
        void initializeData(Data* data);
        void updateData(Data* data);
        void debugPrintData(Data data);
};