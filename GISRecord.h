#ifndef GISRECORD_H
#define GISRECORD_H

#include "GIS.h"

using namespace std;

template <typename T>
class GISRecord {
    struct Record<T> *dms;

    struct World *world;
    int numRecords;


public:
    GISRecord () {

    }

    ~GISRecord() {}
};

#endif