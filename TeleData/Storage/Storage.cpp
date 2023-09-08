#include "Storage.h"
#include "mbed.h"

#include "globalDefines.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
int Storage::initFlashStorage(){
    int err = 0;
    debugPrint("Starting Flash Block Device...\n");
    flashBlockDevice = new SPIFBlockDevice(FLASH_MOSI,FLASH_MISO,FLASH_SCLK,FLASH_CS);
    debugPrint("Starting Flash LFS...\n");
    flashFS = new LittleFileSystem2(FLASH_ROOT);
    debugPrint("Mounting LFS with Flash Block device...");
    err = flashFS->mount(flashBlockDevice); //Not mounting
    err?debugPrint("Failed\n"):debugPrint("Ok\n");
    #ifndef FORCE_REFORMAT_FLASH
    if(err){
        debugPrint("Flash did not mount, reformatting...");
        err = LittleFileSystem2::format(flashBlockDevice);
        err?debugPrint("Failed!\n"):debugPrint("Ok!\n");
    }
    #else
    debugPrint("Force reformat on, reformatting...");
    err = LittleFileSystem2::format(flashBlockDevice);
    err?debugPrint("Failed!\n"):debugPrint("Ok!\n");
    #endif
    debugPrint("FILE: %s\n",nextFile(true));
    return err;
}

void Storage::createFlashFile(){
    filenameFlash = (char*)malloc(FILENAME_SIZE*sizeof(char));
    strcpy(filenameFlash,FLASH_ROOT_DIR);
    strcat(filenameFlash,nextFile(true));
    fFlash = fopen(filenameFlash, "w+");
}

char* Storage::nextFile(bool isFlash){
    char* next_filename;
    char number[4];
    uint16_t current_filename_index = 0;
    next_filename = (char*)malloc(FILENAME_MAX*sizeof(char));
    if(isFlash)
        dir = opendir(FLASH_ROOT_DIR);
    else
        dir = opendir(SD_ROOT_DIR);
    
    debugPrint("Reading directory %s...\n",FLASH_ROOT_DIR);
    while(1){  
        // Defaults to log000.bin, where each 0 can be a number from 0 to 9, starting at 001
        struct dirent *entity = readdir(dir);
        if(!entity)
            break;
        debugPrint("%s\n",entity->d_name);
        if(_CHECK_LOGFILE(entity->d_name,"log")){
            // This pointer points for the char space + 3, being "log" the first 3
            char *ptr = entity->d_name + 3;
            uint16_t i=0;
            while (*ptr >= '0' && *ptr <= '9' && i < 3) {
                number[i] = *ptr;
                i++;
                ptr++;
            }
            number[i] = '\0';
            i = atoi(number);
            if(i > current_filename_index)
                current_filename_index = i;
        }
    }

    if(isFlash)
        fFlash = fopen(FLASH_ROOT_DIR, "w+");
    else
        fSD = fopen(SD_ROOT_DIR,"w+");

    sprintf(next_filename,"log%03d.bin",++current_filename_index);
    return next_filename;
}

void Storage::closeFlashFile(){
    fclose(fFlash);
}

void Storage::deinitFlashStorage(){
    closeFlashFile();
    flashFS->unmount();
    // Frees filename string space in memory
    free(filenameFlash);
}