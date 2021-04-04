#ifndef GIS_H
#define GIS_H

//#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <random>
#include <ctime>
#include <cstring>
#include <set>

#include <algorithm>    // used for counting line numbers in a file

#include<map>

#define INT_PART 3
#define DEC_PART 2
#define INITIAL_TABLE_SIZE  1024
#define BUFFER_SIZE 15

#define INFO "Course Project for COMP 8042\nStudent Name: Kuanysh Boranbayev, Student Id: A00978727\n"

using namespace std;

enum Command {WORLD, IMPORT, DEBUG, QUIT, WHAT_IS, WHAT_IS_AT, WHAT_IS_IN, IGNORE};



struct World {
    long w_long;
    long e_long;
    long n_lat;
    long s_lat;
};

template <typename K>
struct Record {
    long feature_id;
    K feature_name;
    K feature_class;
    K state_alpha;
    K state_numeric;
    K county_name;
    K county_numeric;
    K prim_lat_dms;
    K prim_long_dms;
    long prim_lat_dec;
    long prim_long_dec;
    K source_lat_dms;
    K source_long_dms;
    long source_lat_dec;
    long source_long_dec;
    K elev_in_m;
    K elev_in_ft;
    K map_name;
    K date_created;
    K date_edited;
};

template <typename K>
struct RecordIndex {
    Record<K> record;
    int index;
};


string printDMS (string dms) {
    string res;
    string dayS, minS, secS;
    string NorthSouthWestEast;

    if (dms.at(dms.length() - 1) == 'W')
        NorthSouthWestEast = "West";
    if (dms.at(dms.length() - 1) == 'E')
        NorthSouthWestEast = "East";
    if (dms.at(dms.length() - 1) == 'N')
        NorthSouthWestEast = "North";
    if (dms.at(dms.length() - 1) == 'S')
        NorthSouthWestEast = "South";

    dms.pop_back();
    if (dms.at(0) == '0') {
        //cout << "Zero at front " << endl;
        dms.replace(0,1,"");
    }
    
    int d = 1, m = 0, s = 0;
    long day = 0, min = 0, sec = 0;
    int decimal = 1;
    for (unsigned int i = 0; i < (unsigned int) dms.size(); i++) {
        if (d) {
            if (decimal) {
                day += (int)(dms[i] - '0') * 10;
                decimal = 0;
            } else {
                day += (int)(dms[i] - '0');
                dayS = to_string(day);
                decimal = 1;
                d = 0;
                m = 1;
                day *= 3600;
            }
        } else if (m) {
            if (decimal) {
                min += (int)(dms[i] - '0') * 10;
                decimal = 0;
            } else {
                min += (int)(dms[i] - '0');
                minS = to_string(min);
                decimal = 1;
                s = 1;
                m = 0;
                min *= 60;
            }
        } else if (s) {
            if (decimal) {
                sec += (int)(dms[i] - '0') * 10;
                decimal = 0;
            } else {
                sec += (int)(dms[i] - '0');
                secS = to_string(sec);
                s = 0;
            }
        }
    }

    res = dayS + "d " + minS + "m " + secS + "s " + NorthSouthWestEast;
    return res;
}

long dms2sec (string dms) {
    long res = 0;
    
    int sign = 1;
    
    if (dms.at(dms.length() - 1) == 'W' || dms.at(dms.length() - 1) == 'S')
        sign = -1;
    //string st = dms.substr(0, dms.size() - 1);
    dms.pop_back();
    if (dms.at(0) == '0') {
        //cout << "Zero at front " << endl;
        dms.replace(0,1,"");
    }
    
    int d = 1, m = 0, s = 0;
    long day = 0, min = 0, sec = 0;
    int decimal = 1;
    for (unsigned int i = 0; i < (unsigned int) dms.size(); i++) {
        if (d) {
            if (decimal) {
                day += (int)(dms[i] - '0') * 10;
                decimal = 0;
            } else {
                day += (int)(dms[i] - '0');
                decimal = 1;
                d = 0;
                m = 1;
                day *= 3600;
                res += day;
            }
        } else if (m) {
            if (decimal) {
                min += (int)(dms[i] - '0') * 10;
                decimal = 0;
            } else {
                min += (int)(dms[i] - '0');
                decimal = 1;
                s = 1;
                m = 0;
                min *= 60;
                res += min;
            }
        } else if (s) {
            if (decimal) {
                sec += (int)(dms[i] - '0') * 10;
                decimal = 0;
            } else {
                sec += (int)(dms[i] - '0');
                s = 0;
                res += sec;
            }
        }
    }

    res = res * sign;

    return res;
}

#endif