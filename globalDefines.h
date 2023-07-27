/* RADIO DEFINES */

#define LORA_BANDWIDTH LORA_BW_031              // LoRa padrão, detalhes em SX1276::BandwidthMap
#define LORA_SPREADING_FACTOR LORA_SF10         // Spreading Factor
#define LORA_CODINGRATE LORA_CR_4_5
#define LORA_DATARATE LORA_DATARATE_2048    //Settado em 2048, mas deve ser tunado no futuro
#define LORA_FIX_LEN false
#define LORA_ENABLE_CRC true
#define LORA_ENABLE_FHSS false
#define LORA_FHSS_PERIOD 4
#define LORA_PREAMBLE_LENGTH 0x8
#define LORA_INVERT_IQ false
#define LORA_TIMEOUT_PERIOD 2000 //ms
#define TX_OUTPUT_POWER 20 // Em dBm

#define RF_FREQUENCY 915e6