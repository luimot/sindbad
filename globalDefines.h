/* RADIO DEFINES */

#define LORA_BANDWIDTH LORA_BW_031              // details in SX1276::BandwidthMap
#define LORA_SPREADING_FACTOR LORA_SF10
#define LORA_CODINGRATE LORA_CR_4_5
#define LORA_DATARATE LORA_DATARATE_2048    // In 2048, should change in the future
#define LORA_FIX_LEN false
#define LORA_ENABLE_CRC true
#define LORA_ENABLE_FHSS false
#define LORA_FHSS_PERIOD 4
#define LORA_PREAMBLE_LENGTH 0x8
#define LORA_INVERT_IQ false
#define LORA_TIMEOUT_PERIOD 2000 //ms
#define TX_OUTPUT_POWER 20 // Em dBm

#define RF_FREQUENCY 915e6

/* GLOBAL POSITIONING DEFINES */

#define GPS_EN
#define GPS_UART

#ifdef GPS_EN
#ifdef GPS_UART
/* GPS UBLOX DEFINES (UART) */

#define GPS_TX PB_6
#define GPS_RX PB_7
#define GPS_BAUDRATE 9600
#else
/* GPS UBLOX DEFINES (I2C) */
#define GPS_SDA PA_11
#define GPS_SCL PA_12

#endif
#endif

/* PRINT OPTION DEFINES */

#define DEBUG_MESSAGES

// If debug messages are active, debugPrint is used as printf, otherwise it
// will be treated as an empty function
#ifdef DEBUG_MESSAGES
#define debugPrint(...) printf(__VA_ARGS__)
#else
#define debugPrint(...)
#endif