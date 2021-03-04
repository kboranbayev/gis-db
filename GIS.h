#ifndef GIS_H
#define GIS_H

#include <iostream>
#include <fstream>
#include <sstream>
//#include <gtest/gtest.h>
#include <iomanip>
#include <math.h>
#include <random>
#include <ctime>

#define INFO "Course Project for COMP 8042\nStudent Name: Kuanysh Boranbayev, Student Id: A00978727\n"

enum Command {WORLD, IMPORT, DEBUG, QUIT, WHAT_IS_AT, WHAT_IS_IN, IGNORE};


template <typename T>
struct Record {
    long feature_id;
    T feature_name;
    T feature_class;
    T state_alpha;
    T state_numeric;
    T county_name;
    T county_numeric;
    T primary_lat_dms;
    T prim_long_dms;
    T prim_lat_dec;
    T prim_long_dec;
    T source_lat_dms;
    T source_long_dms;
    T source_lat_dec;
    T source_long_dec;
    T elev_in_m;
    T elev_in_ft;
    T map_name;
    T date_created;
    T date_edited;
};

double deg2sec (double  degree, int minute, int second) {
    return degree * 3600 + minute * 60 + second;
}
#endif