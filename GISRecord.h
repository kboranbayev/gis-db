#ifndef GISRECORD_H
#define GISRECORD_H

#include "GIS.h"

using namespace std;

template <typename K>
class GISRecord {
    //struct vector<Record<K>> *dms;

    //struct World * world;
    //int numRecords;

    long feature_id;
    K feature_name;
    K feature_class;
    K state_alpha;
    K state_numeric;
    K county_name;
    K county_numeric;
    K prim_lat_dms;
    K prim_long_dms;
    double prim_lat_dec;
    double prim_long_dec;
    K source_lat_dms;
    K source_long_dms;
    double source_lat_dec;
    double source_long_dec;
    K elev_in_m;
    K elev_in_ft;
    K map_name;
    K date_created;
    K date_edited;

public:
    GISRecord  (string f_id, string f_name, string f_class, 
                string s_alpha, string s_numeric, 
                string c_name, string c_numeric,
                string p_lat_dms, string p_long_dms, string p_lat_dec, string p_long_dec,
                string s_lat_dms, string s_long_dms, string s_lat_dec, string s_long_dec,
                string e_in_m, string e_in_ft, string m_name,
                string d_created, string d_edited) 
    {  

    }

    ~GISRecord() {}

    void insert() {}

};

#endif