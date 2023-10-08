#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "mbed.h"
#include "blockdevice/COMPONENT_SPIF/include/SPIF/SPIFBlockDevice.h"
#include "LittleFileSystem2.h"
#include "Data.h"
#include "globalDefines.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#define FLASH_ROOT "f"
#define FLASH_ROOT_DIR "/f/"
#define SD_ROOT "sd"
#define SD_ROOT_DIR "/sd/"

#define FILENAME_SIZE 64

/* STORAGE FUNCTION-LIKE MACROS */
#define _CHECK_LOGFILE(filename, s) filename[0] == s[0] && filename[1] == s[1] && filename[2] == s[2]   

class Storage{
    private:
        SPIFBlockDevice *flashBlockDevice;
        SDBlockDevice *sdBlockDevice;
        LittleFileSystem2 *sdFS, *flashFS;
        FILE *fSD, *fFlash;
        DIR *dir;
        char *filenameSD, *filenameFlash;
        char* nextFile(bool iFlash);

    public:
        /* FLASH STORAGE METHODS */        
        int initFlashStorage();
        void eraseFlash();
        void openFlashFile();
        void readFlashFile(char* path);
        void closeFlashFile();
        void createFlashFile();
        uint16_t writeFlashFile(Data data);
        void deinitFlashStorage();
        /* SD CARD STORAGE METHODS */
        int initSDStorage();
        void eraseSD();
        void openSDFile();
        void readSDFile(char* path);
        void closeSDFile();
        void createSDFile();
        uint16_t writeSDFile(Data data);
        void deinitSDStorage();
};

typedef struct{
    SPIFBlockDevice *blockDevice;
    LittleFileSystem2 *flashFS;
    FILE *file;

} FlashDevice;

typedef struct{
    SDBlockDevice *blockDevice;
    FATFileSystem *sdFS;
    FILE *file;
} SDdevice;