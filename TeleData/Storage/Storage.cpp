#include "Storage.h"
#include "FATFileSystem.h"
#include <cstdio>

template<>
int Storage::initStorage<FlashDevice>(){
    int err = 0;
    debugPrint("Starting Flash Block Device...\n");
    flash.blockDevice = new SPIFBlockDevice(FLASH_MOSI,FLASH_MISO,FLASH_SCLK,FLASH_CS);
    debugPrint("Starting Flash LFS...\n");
    flash.fs = new LittleFileSystem2(FLASH_ROOT);
    debugPrint("Mounting LFS with Flash Block device...");
    err = flash.fs->mount(flash.blockDevice); //Not mounting
    err?debugPrint("Failed\n"):debugPrint("Ok\n");
    #ifndef FORCE_REFORMAT_FLASH
    if(err){
        debugPrint("Flash did not mount, reformatting...");
        err = LittleFileSystem2::format(flash.blockDevice);
        err?debugPrint("Failed!\n"):debugPrint("Ok!\n");
    }
    #else
    debugPrint("Force reformat on, reformatting...");
    err = LittleFileSystem2::format(flash.blockDevice);
    err?debugPrint("Failed!\n"):debugPrint("Ok!\n");
    #endif
    return err;
}

template<>
int Storage::initStorage<SDdevice>(){
    int err = 0;
    debugPrint("Starting SD Block Device...\n");
    sd.blockDevice = new SDBlockDevice(SD_MOSI,SD_MISO,SD_SCLK,SD_CS);
    debugPrint("Starting SD LFS...\n");
    sd.fs = new FATFileSystem(SD_ROOT);
    debugPrint("Mounting FAT FS with SD Block device...");
    err = sd.fs->mount(sd.blockDevice); //Not mounting
    err?debugPrint("Failed\n"):debugPrint("Ok\n");
    #ifndef FORCE_REFORMAT_SD
    if(err){
        debugPrint("SD did not mount, reformatting...");
        err = FATFileSystem::format(sd.blockDevice);
        err?debugPrint("Failed!\n"):debugPrint("Ok!\n");
    }
    #else
    debugPrint("Force reformat on, reformatting...");
    err = FATFileSystem::format(sd.blockDevice);
    err?debugPrint("Failed!\n"):debugPrint("Ok!\n");
    #endif
    return err;
}

template<>
void Storage::eraseStorage<FlashDevice>(){
    flash.blockDevice->erase(0,flash.blockDevice->size());    // Full erase
}

template<>
void Storage::eraseStorage<SDdevice>(){
    sd.blockDevice->erase(0,sd.blockDevice->size());    // Full erase
}

void Storage::openFile(StorageDevice* storage){
    storage->file = fopen(storage->rootDir(),"w+");
}

void Storage::closeFile(StorageDevice *storage){
    fclose(storage->file);
}

uint16_t Storage::writeFile(Data data, FILE* file){
    uint16_t writeSize = 0;
    if(file == nullptr){
        debugPrint("Could not write on file. Perhaps the file is not open\n");
    }
    else{
        writeSize = fwrite((char*)&data, sizeof(char), sizeof(Data), file);
    }
    return writeSize;
}

char* Storage::nextFile(StorageDevice* storage){
    char* next_filename;
    char number[4];
    uint16_t current_filename_index = 0;
    next_filename = (char*)malloc(FILENAME_MAX*sizeof(char));
    dir = opendir(storage->rootDir());
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

    openFile(storage);  //Why am I opening this again?

    if(current_filename_index < 1000)
        sprintf(next_filename,"log%03d.bin",++current_filename_index);
    else
        sprintf(next_filename,"nope");
    return next_filename;
}

void Storage::createFile(StorageDevice storage){
    char* nextFName = nextFile(&storage);
    if(strcmp(nextFName,"nope") == 0){
        // TODO: Treat error, stop and beep error
    }
    storage.filename = (char*)malloc(FILENAME_SIZE*sizeof(char));
    strcpy(storage.filename,FLASH_ROOT_DIR);
    strcat(storage.filename,nextFName);
    storage.file = fopen(storage.filename, "w+");
}

char* Storage::getCurrentPath(StorageDevice storage){
    char* path;
    strcpy(path,storage.rootDir());
    strcat(path,storage.filename);
    return path;
}

void Storage::readFile(StorageDevice* storage){
    char* path = getCurrentPath(*storage); // TODO: Assign path
    debugPrint("Displaying \"%s\"...", path); // Like "/fs/log002.txt" 
    fflush(stdout); // Cleans the standard output buffer
    if(storage->file != nullptr){
        debugPrint("There's a file already open, trying to open \"%s\"...", path);
        closeFile(storage);
    }
    storage->file = fopen(path,"r");
    if(!storage->file)
        debugPrint("Could not read Flash file. Perhaps the file is not created\n");
    else{
        filePrint("Preamble, Pressure, Temperature, Humdity, gpsFix, Latitude, Longitude, Time, TimeStamp, Acc(x), Acc(y), Acc(z)\n");
        Data tmpData;
        while(!feof(storage->file)){   // While EOF(End Of File) is not found
            fread((char* )&tmpData, sizeof(char), sizeof(Data),storage->file);
            filePrint("%s,%f,%f,%f,%d,%f,%f,%f,%d,%lld,%d,%d,%d\n",
               tmpData.preamble,
               tmpData.pressure,
               tmpData.temperatureLPS22HB,
               tmpData.humidity,
               tmpData.gpsFix,
               tmpData.latitude,
               tmpData.longitude,
               tmpData.elevation,
               tmpData.UTCTime,
               tmpData.timeStamp,
               tmpData.a[0],
               tmpData.a[1],
               tmpData.a[2]);
        }
        fflush(stdout);
        closeFile(storage);
    }
}