#include "mbed.h"
#include "blockdevice/COMPONENT_SPIF/include/SPIF/SPIFBlockDevice.h"
#include "LittleFileSystem2.h"

#define FLASH_ROOT "f"
#define FLASH_ROOT_DIR "/f/"
#define SD_ROOT "sd"
#define SD_ROOT_DIR "/sd/"

#define FILENAME_SIZE 64

#define _CHECK_LOGFILE(filename, s) filename[0] == s[0] && filename[1] == s[1] && filename[2] == s[2]   

class Storage{
    private:
        SPIFBlockDevice *sdBlockDevice, *flashBlockDevice;
        LittleFileSystem2 *sdFS, *flashFS;
        FILE *fSD, *fFlash;
        DIR *dir;
        char *filenameSD, *filenameFlash;
        char* nextFile(bool iFlash);

    public:
        int initFlashStorage();
        void closeFlashFile();
        void createFlashFile();
        void writeFlashFile();
        void deinitFlashStorage();
};