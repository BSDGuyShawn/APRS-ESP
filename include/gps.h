/*
    Description:    This file is part of the APRS-ESP project.
                    This file contains the code for the GPS processing.
    Author:         Ernest / LY3PH
    License:        GNU General Public License v3.0
    Includes code from:
                    https://github.com/sh123/aprs_tracker
*/

#ifndef GNSS_H
#define GNSS_H

#include <Arduino.h>
#include "main.h"

// APRS update heuristics
enum t_update_heuristics {
    H_OFF = 0,
    H_PERIODIC_1MIN,
    H_PERIODIC_5MIN,
    H_PERIODIC_10MIN,
    H_PERIODIC_30MIN,
    H_RANGE_500M,
    H_RANGE_1KM,
    H_RANGE_5KM,
    H_SMART_BEACONING,
    H_MAX_HEURISTICS = H_SMART_BEACONING
};

extern long lat;
extern long lon;
extern long lat_prev;
extern long lon_prev;
extern long distance;
extern unsigned long age;

extern bool send_aprs_update;

extern int8_t active_heuristic;
extern int8_t selected_heuristic;

void GpsUpdate();
void heuristicDistanceChanged();

float conv_coords(float in_coords);
void DD_DDDDDtoDDMMSS(float DD_DDDDD, int *DD, int *MM, int *SS);

char *deg_to_nmea(long deg, boolean is_lat);
char *deg_to_qth(long lat, long lon);

#endif // GNSS_H
