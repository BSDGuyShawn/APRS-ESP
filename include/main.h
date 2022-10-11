/*
 Name:		ESP32 APRS Internet Gateway / Tracker / Digipeater
 Created:	2022-10-10 / initially 1-Nov-2021 14:27:23
 Author:	LY3PH/Ernest / initially HS5TQA/Atten
*/

#ifndef MAIN_H
#define MAIN_H

#define VERSION "0.1"

#define DEBUG
//#define DEBUG_IS

//#define SDCARD
//#define USE_TNC
#define USE_GPS
#define USE_SCREEN
//#define USE_BLE
//#define USE_KISS  // disables tracker, enables kiss serial modem mode
//#define USE_ROTARY

#ifdef USE_SA828
#define APRS_PREAMBLE	(350UL * 3)
#define APRS_TAIL       (250UL)
#else
#define APRS_PREAMBLE	(350UL)
#define APRS_TAIL       (0UL)
#endif

#define TNC_TELEMETRY_PERIOD    600000UL    // 10 minutes

#if defined(USE_SA818) || defined(USE_SA828) || defined(USE_SR_FRS)
#define USE_RF
#endif

#if defined(USE_TNC) && defined(USE_GPS)
#error "Cannot use both USE_TNC and USE_GPS"
#endif

#ifdef SR_FRS
#ifndef USE_SA818
#define USE_SA818
#endif
#endif

#include "pinout.h"

#define WIFI_OFF_FIX 0
#define WIFI_AP_FIX 1
#define WIFI_STA_FIX 2
#define WIFI_AP_STA_FIX 3

#define IMPLEMENTATION FIFO

#define TZ 0      // (utc+) TZ in hours
#define DST_MN 0  // use 60mn for summer time in some countries
#define TZ_MN ((TZ)*60)
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)

#define FORMAT_SPIFFS_IF_FAILED true

#define PKGLISTSIZE 10
#define PKGTXSIZE 5

const int timeZone = 7;  // Bangkok

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>
#include "soc/rtc_wdt.h"
#include <AX25.h>

#include "HardwareSerial.h"
#include "EEPROM.h"

enum M17Flags {
    DISCONNECTED = 1 << 0,
    CONNECTING = 1 << 1,
    M17_AUTH = 1 << 2,
    M17_CONF = 1 << 3,
    M17_OPTS = 1 << 4,
    CONNECTED_RW = 1 << 5,
    CONNECTED_RO = 1 << 6
};

typedef struct Config_Struct {
    bool synctime;
    bool aprs;
    bool wifi_client;
    bool wifi;
    char wifi_mode;  // WIFI_AP,WIFI_STA,WIFI_AP_STA,WIFI_OFF
    float gps_lat;
    float gps_lon;
    float gps_alt;
    uint8_t aprs_ssid;
    uint16_t aprs_port;
    uint8_t aprs_moniSSID;
    uint32_t api_id;
    bool tnc;
    bool rf2inet;
    bool inet2rf;
    bool tnc_digi = false;
    bool tnc_telemetry = false;
    int tnc_beacon = 0;
    int aprs_beacon;
    char aprs_table;
    char aprs_symbol;
    char aprs_mycall[10];
    char aprs_host[20];
    char aprs_passcode[10];
    char aprs_moniCall[10];
    char aprs_filter[30];
    char aprs_comment[50];
    char aprs_path[72];
    char wifi_ssid[32];
    char wifi_pass[63];
    char wifi_ap_ssid[32];
    char wifi_ap_pass[63];
    char tnc_path[50];
    char tnc_btext[50];
    char tnc_comment[50];
    char aprs_object[10];
    char wifi_power;
    uint16_t tx_timeslot;
    uint16_t digi_delay;
    bool input_hpf;
#ifdef USE_RF
    float freq_rx;
    float freq_tx;
    int offset_rx;
    int offset_tx;
    int tone_rx;
    int tone_tx;
    uint8_t band;
    uint8_t sql_level;
    bool rf_power;
    uint8_t volume;
#endif
    int8_t timeZone;

} Configuration;

typedef struct igateTLM_struct {
    uint16_t Sequence;
    unsigned long ParmTimeout;
    unsigned long TeleTimeout;
    uint8_t RF2INET;
    uint8_t INET2RF;
    uint8_t RX;
    uint8_t TX;
    uint8_t DROP;
} igateTLMType;

typedef struct statisticStruct {
    uint32_t allCount;
    uint32_t tncCount;
    uint32_t isCount;
    uint32_t locationCount;
    uint32_t wxCount;
    uint32_t digiCount;
    uint32_t errorCount;
    uint32_t dropCount;
    uint32_t rf2inet;
    uint32_t inet2rf;
} statusType;

typedef struct digiTLM_struct {
    unsigned int Sequence;
    unsigned int ParmTimeout;
    unsigned int TeleTimeout;
    unsigned char RxPkts;
    unsigned char TxPkts;
    unsigned char DropRx;
    unsigned char ErPkts;
} digiTLMType;

typedef struct txQueue_struct {
    bool Active;
    long timeStamp;
    int Delay;
    char Info[300];
} txQueueType;

const char PARM[] = {"PARM.RF->INET,INET->RF,TxPkts,RxPkts,IGateDropRx"};
const char UNIT[] = {"UNIT.Pkts,Pkts,Pkts,Pkts,Pkts"};
const char EQNS[] = {"EQNS.0,1,0,0,1,0,0,1,0,0,1,0,0,1,0"};

const float ctcss[] = {0,     67,    71.9,  74.4,  77,    79.7,  82.5,  85.4,
                       88.5,  91.5,  94.8,  97.4,  100,   103.5, 107.2, 110.9,
                       114.8, 118.8, 123,   127.3, 131.8, 136.5, 141.3, 146.2,
                       151.4, 156.7, 162.2, 167.9, 173.8, 179.9, 186.2, 192.8,
                       203.5, 210.7, 218.1, 225.7, 233.6, 241.8, 250.3};
const float wifiPwr[12][2] = {{-4, -1},  {8, 2},     {20, 5},  {28, 7},
                              {34, 8.5}, {44, 11},   {52, 13}, {60, 15},
                              {68, 17},  {74, 18.5}, {76, 19}, {78, 19.5}};

void saveEEPROM();
void defaultConfig();
String getValue(String data, char separator, int index);
boolean isValidNumber(String str);
void taskAPRS(void *pvParameters);
void taskNetwork(void *pvParameters);
int processPacket(String &tnc2);
String send_fix_location();
int digiProcess(AX25Msg &Packet);
void printTime();
bool pkgTxUpdate(const char *info, int delay);

#endif
