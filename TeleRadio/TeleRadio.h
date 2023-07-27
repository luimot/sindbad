#include "Data.h"

#include "lorawan/LoRaRadio.h"

#if COMPONENT_SX1272
#include "SX1272_LoRaRadio.h"
#elif COMPONENT_SX1276
#include "SX1276_LoRaRadio.h"

#elif COMPONENT_SX126X
#include "SX126X_LoRaRadio.h"

#elif (TARGET_STM32WL)
#include "STM32WL_LoRaRadio.h"
typedef enum{
    LORA_DATARATE_64= 6,
    LORA_DATARATE_128= 7,
    LORA_DATARATE_256= 8,
    LORA_DATARATE_512= 9,
    LORA_DATARATE_1024= 10,
    LORA_DATARATE_2048= 11,
    LORA_DATARATE_4096= 12,

} radio_datarate_t;
#else
#error "Unknown LoRa radio specified (SX126X, SX1272, SX1276, STM32WL are valid)"
#endif

class TeleRadio{
public:

#if COMPONENT_SX1272
SX1272_LoRaRadio radio(MBED_CONF_SX1272_LORA_DRIVER_SPI_MOSI,
                       MBED_CONF_SX1272_LORA_DRIVER_SPI_MISO,
                       MBED_CONF_SX1272_LORA_DRIVER_SPI_SCLK,
                       MBED_CONF_SX1272_LORA_DRIVER_SPI_CS,
                       MBED_CONF_SX1272_LORA_DRIVER_RESET,
                       MBED_CONF_SX1272_LORA_DRIVER_DIO0,
                       MBED_CONF_SX1272_LORA_DRIVER_DIO1,
                       MBED_CONF_SX1272_LORA_DRIVER_DIO2,
                       MBED_CONF_SX1272_LORA_DRIVER_DIO3,
                       MBED_CONF_SX1272_LORA_DRIVER_DIO4,
                       MBED_CONF_SX1272_LORA_DRIVER_DIO5,
                       MBED_CONF_SX1272_LORA_DRIVER_RF_SWITCH_CTL1,
                       MBED_CONF_SX1272_LORA_DRIVER_RF_SWITCH_CTL2,
                       MBED_CONF_SX1272_LORA_DRIVER_TXCTL,
                       MBED_CONF_SX1272_LORA_DRIVER_RXCTL,
                       MBED_CONF_SX1272_LORA_DRIVER_ANT_SWITCH,
                       MBED_CONF_SX1272_LORA_DRIVER_PWR_AMP_CTL,
                       MBED_CONF_SX1272_LORA_DRIVER_TCXO);

#elif COMPONENT_SX1276
SX1276_LoRaRadio radio(MBED_CONF_SX1276_LORA_DRIVER_SPI_MOSI,
                       MBED_CONF_SX1276_LORA_DRIVER_SPI_MISO,
                       MBED_CONF_SX1276_LORA_DRIVER_SPI_SCLK,
                       MBED_CONF_SX1276_LORA_DRIVER_SPI_CS,
                       MBED_CONF_SX1276_LORA_DRIVER_RESET,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO0,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO1,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO2,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO3,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO4,
                       MBED_CONF_SX1276_LORA_DRIVER_DIO5,
                       MBED_CONF_SX1276_LORA_DRIVER_RF_SWITCH_CTL1,
                       MBED_CONF_SX1276_LORA_DRIVER_RF_SWITCH_CTL2,
                       MBED_CONF_SX1276_LORA_DRIVER_TXCTL,
                       MBED_CONF_SX1276_LORA_DRIVER_RXCTL,
                       MBED_CONF_SX1276_LORA_DRIVER_ANT_SWITCH,
                       MBED_CONF_SX1276_LORA_DRIVER_PWR_AMP_CTL,
                       MBED_CONF_SX1276_LORA_DRIVER_TCXO);

#elif COMPONENT_SX126X
SX126X_LoRaRadio radio(MBED_CONF_SX126X_LORA_DRIVER_SPI_MOSI,
                       MBED_CONF_SX126X_LORA_DRIVER_SPI_MISO,
                       MBED_CONF_SX126X_LORA_DRIVER_SPI_SCLK,
                       MBED_CONF_SX126X_LORA_DRIVER_SPI_CS,
                       MBED_CONF_SX126X_LORA_DRIVER_RESET,
                       MBED_CONF_SX126X_LORA_DRIVER_DIO1,
                       MBED_CONF_SX126X_LORA_DRIVER_BUSY,
                       MBED_CONF_SX126X_LORA_DRIVER_FREQ_SELECT,
                       MBED_CONF_SX126X_LORA_DRIVER_DEVICE_SELECT,
                       MBED_CONF_SX126X_LORA_DRIVER_CRYSTAL_SELECT,
                       MBED_CONF_SX126X_LORA_DRIVER_ANT_SWITCH);

#elif (TARGET_STM32WL)
STM32WL_LoRaRadio radio;
#endif
    
    radio_events_t radioEvents;

    Data data;
    void setEvents();
    void init();
    uint8_t radioStatus();
    void send(Data *txData);
    int16_t receivedRSSI;

    /* Radio Events */
    void OnRxDone(const uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);




};
