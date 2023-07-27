#include "TeleRadio.h"
#include "globalDefines.h"


void TeleRadio::setEvents(){
    //Set of lambda functions acting as callbacks to the radio events sensed by the radio
    radioEvents.tx_done = [this](){radio.standby();};
    radioEvents.rx_done = [this](const uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr){
        radio.standby();
        receivedRSSI = rssi;
        memcpy(&data, payload, sizeof(Data));
    };
    
}

void TeleRadio::init(){
    setEvents();
    radio.lock();
    radio.init_radio(&radioEvents);
    radio.unlock();
    radio.set_channel(RF_FREQUENCY);
    radio.set_tx_config(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                        LORA_CODINGRATE, LORA_PREAMBLE_LENGTH, LORA_FIX_LEN, LORA_ENABLE_CRC,
                        LORA_ENABLE_FHSS, LORA_FHSS_PERIOD, LORA_INVERT_IQ, LORA_TIMEOUT_PERIOD);
    radio.set_max_payload_length(MODEM_LORA, sizeof(Data));
}

void TeleRadio::send(Data *txData){
    radio.send((uint8_t *)txData, sizeof(*txData));
}

uint8_t TeleRadio::radioStatus() {return radio.get_status();}

