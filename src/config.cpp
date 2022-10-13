/*
    Description:    This file is part of the APRS-ESP project.
                    This file contains the code for work with Configuration, stored in EEPROM.
    Author:         Ernest (ErNis) / LY3PH
    License:        GNU General Public License v3.0
    Includes code from:
                    https://github.com/nakhonthai/ESP32IGate
*/

#include "config.h"

extern Configuration config;
extern bool input_HPF;

uint8_t checkSum(uint8_t *ptr, size_t count) {
    uint8_t lrc, tmp;
    uint16_t i;
    lrc = 0;
    for (i = 0; i < count; i++) {
        tmp = *ptr++;
        lrc = lrc ^ tmp;
    }
    return lrc;
}

void SaveConfig() {
    uint8_t chkSum = 0;
    byte *ptr;
    ptr = (byte *)&config;
    EEPROM.writeBytes(1, ptr, sizeof(Configuration));
    chkSum = checkSum(ptr, sizeof(Configuration));
    EEPROM.write(0, chkSum);
    EEPROM.commit();
#ifdef DEBUG
    Serial.print("Save EEPROM ChkSUM=");
    Serial.println(chkSum, HEX);
#endif
}

void DefaultConfig() {
    Serial.println("Applying Factory Default configuration!");
    sprintf(config.aprs_mycall, "MYCALL");
    config.aprs_ssid = 15;
    sprintf(config.aprs_host, "rotate.aprs2.net");
    config.aprs_port = 14580;
    sprintf(config.aprs_passcode, "00000");
    sprintf(config.aprs_moniCall, "%s-%d", config.aprs_mycall, config.aprs_ssid);
    sprintf(config.aprs_filter, "g/HS*/E2*");
    sprintf(config.wifi_ssid, "APRS-ESP32");
    sprintf(config.wifi_pass, "aprs");
    sprintf(config.wifi_ap_ssid, "APRS-ESP32");
    sprintf(config.wifi_ap_pass, "aprs");
    config.wifi_client = true;
    config.synctime = true;
    config.aprs_beacon = 600;
    config.gps_lat = 54.6842;
    config.gps_lon = 25.2398;
    config.gps_alt = 10;
    config.tnc = true;
    config.inet2rf = true;
    config.rf2inet = true;
    config.aprs = false;
    config.wifi = true;
    config.wifi_mode = WIFI_AP_STA_FIX;
    config.tnc_digi = true;
    config.tnc_telemetry = true;
    config.tnc_btext[0] = 0;
    config.tnc_beacon = 0;
    config.aprs_table = '/';
    config.aprs_symbol = '&';
    config.digi_delay = 2000;
    config.tx_timeslot = 5000;
    sprintf(config.aprs_path, "WIDE1-1");
    sprintf(config.aprs_comment, "APRS-ESP Internet Gateway");
    sprintf(config.tnc_comment, "APRS-ESP Built in TNC");
    sprintf(config.aprs_filter, "g/HS*/E2*");
    sprintf(config.tnc_path, "WIDE1-1");
    config.wifi_power = 44;
    config.input_hpf = true;
#ifdef USE_RF
    config.freq_rx = 144.8000;
    config.freq_tx = 144.8000;
    config.offset_rx = 0;
    config.offset_tx = 0;
    config.tone_rx = 0;
    config.tone_tx = 0;
    config.band = 0;
    config.sql_level = 1;
    config.rf_power = LOW;
    config.volume = 4;
    config.input_hpf = false;
#endif
    input_HPF = config.input_hpf;
    config.timeZone = 0;
    SaveConfig();
}

void LoadConfig() {
    byte *ptr;

    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println(F("failed to initialise EEPROM"));  // delay(100000);
    }

    delay(3000);
    if (digitalRead(BOOT_PIN) == LOW) {
        DefaultConfig();
        Serial.println("Restoring Factory Default config");
        while (digitalRead(BOOT_PIN) == LOW)
            ;
    }

    // Check for configuration errors
    ptr = (byte *)&config;
    EEPROM.readBytes(1, ptr, sizeof(Configuration));
    uint8_t chkSum = checkSum(ptr, sizeof(Configuration));
    Serial.printf("EEPROM Check %0Xh=%0Xh(%dByte)\n", EEPROM.read(0), chkSum,
                  sizeof(Configuration));
    if (EEPROM.read(0) != chkSum) {
        Serial.println("Config EEPROM Error!");
        DefaultConfig();
    }
    input_HPF = config.input_hpf;
}