#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include "GIS.h"

#include "NameIndex.h"

#include <list>

#define BUFFER_SIZE 15

using namespace std;

//template <typename K>
class BufferPool {
    int size;

    // store keys to cache
    list<int> keys;

    vector<string> buffer;

    vector<Record<string>> all;

    //ofstream * db;
    World *world;

    string dbName;

    NameIndex<string> *nameIndex;

public:
    BufferPool (string db_name, World *world_boundaries, string file) {
        ofstream db1(db_name.c_str(), ios::out);
        db1.close();

        if (db1.is_open())
            cout << "It is still open even if I did close.\n";
        //db1.open(dbName.c_str(), ios::app);

        //db = &db1;
        dbName = db_name;
        ifstream inFile(file); 
        
        int n = count(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>(), '\n');

        size = n;
        //all.resize(size);
        inFile.close();

        world = world_boundaries;

        nameIndex = new NameIndex<string>(INITIAL_TABLE_SIZE);
    }

    int getSize(void) {
        return size;
    }


    void readData (string data_file_name) {
        cout << "Importing |" << data_file_name << '|' << endl;

        ifstream data(data_file_name);

        if (data.fail()) {
            cerr << "Import Error: " << data_file_name << " file not found" << endl;
            exit(2);
        }
        string line;

        while (getline(data, line)) {
            istringstream iss(line);
            string feature_id, feature_name, feature_class, state_alpha, state_numeric, county_name, county_numeric;
            string prim_lat_dms, prim_long_dms, prim_lat_dec, prim_long_dec;
            string source_lat_dms, source_long_dms, source_lat_dec, source_long_dec;
            string elec_in_m, elec_in_ft, map_name;
            string date_created, date_edited;
            if (line.rfind("FEATURE_ID|") != 0) {
                getline(iss, feature_id, '|');
                getline(iss, feature_name, '|');
                getline(iss, feature_class, '|');
                getline(iss, state_alpha, '|');
                getline(iss, state_numeric, '|');
                getline(iss, county_name, '|');
                getline(iss, county_numeric, '|');
                getline(iss, prim_lat_dms, '|');
                getline(iss, prim_long_dms, '|');
                getline(iss, prim_lat_dec, '|');
                getline(iss, prim_long_dec, '|');
                getline(iss, source_lat_dms, '|');
                getline(iss, source_long_dms, '|');
                getline(iss, source_lat_dec, '|');
                getline(iss, source_long_dec, '|');
                getline(iss, source_lat_dms, '|');
                getline(iss, source_long_dms, '|');
                getline(iss, elec_in_m, '|');
                getline(iss, elec_in_ft, '|');
                getline(iss, map_name, '|');
                getline(iss, date_created, '|');
                getline(iss, date_edited, '|');
                


                Record<string> r { 
                    atol(feature_id.c_str()),
                    feature_name,
                    feature_class,
                    state_alpha,
                    state_numeric,
                    county_name,
                    county_numeric,
                    prim_lat_dms,
                    prim_long_dms,
                    atof(prim_lat_dec.c_str()),
                    atof(prim_long_dec.c_str()),
                    source_lat_dms,
                    source_long_dms,
                    atof(source_lat_dec.c_str()),
                    atof(source_long_dec.c_str()),
                    elec_in_m,
                    elec_in_ft,
                    map_name,
                    date_created,
                    date_edited 
                };

                if (dms2sec(prim_lat_dms) < world->n_lat && dms2sec(prim_lat_dms) > world->s_lat && 
                    dms2sec(prim_long_dms) < world->e_long && dms2sec(prim_long_dms) > world->w_long) {
                    
                    all.push_back(r);
                }

                

                indexIt();
                
/*
                r.feature_id = atol(feature_id.c_str());
                r.feature_name = feature_name;
                r.feature_class = feature_class;
                r.state_alpha = state_alpha;
                r.state_numeric = state_numeric;
                r.county_name = county_name;
                r.county_numeric = county_numeric;
                r.prim_lat_dms = prim_lat_dms;
                r.prim_long_dms = prim_long_dms;
                r.prim_lat_dec = 
                r.prim_long_dec = 

                //cout << "-------------------------------------------" << endl;
                cout << "F_ID: " << feature_id << endl;
                cout << "F_N: " << feature_name << endl;
                cout << "F_C: " << feature_class << endl;
                cout << "S_A: " << state_alpha << endl;
                cout << "S_N: " << state_numeric << endl;
                cout << "C_NA: " << county_name << endl;
                cout << "C_NU: " << county_numeric << endl;

                cout << "S_LAT_DMS: " << source_lat_dms << endl;
                cout << "S_LON_DMS: " << source_long_dms << endl;

                cout << "F_ID: " << r.feature_id << endl;
                cout << "F_N: " << r.feature_name << endl;
                cout << "F_C: " << r.feature_class << endl;
                cout << "S_A: " << r.state_alpha << endl;
                cout << "S_N: " << r.state_numeric << endl;
                cout << "C_NA: " << r.county_name << endl;
                cout << "C_NU: " << r.county_numeric << endl;

                cout << "S_LAT_DMS: " << r.source_lat_dms << endl;
                cout << "S_LON_DMS: " << r.source_long_dms << endl;

                cout << "-------------------------------------------" << endl;
*/
            }
                
        }
    }

    void indexIt(void) {
        for (int i = 0; i < (int) all.size(); i++) {
            string entry = "[" + all[i].feature_name + ":" + all[i].state_alpha + ", [" + std::to_string(i) + "]]";
            nameIndex->insert(entry);
        }
    }

    void displayAll(void) {
        for (vector<Record<string>>::iterator r = all.begin(); r != all.end(); ++r) {
            
            cout << "F_ID: " << r->feature_id << endl;
            cout << "F_N: " << r->feature_name << endl;
            cout << "F_C: " << r->feature_class << endl;
            cout << "S_A: " << r->state_alpha << endl;
            cout << "S_N: " << r->state_numeric << endl;
            cout << "C_NA: " << r->county_name << endl;
            cout << "C_NU: " << r->county_numeric << endl;

            cout << "S_LAT_DMS: " << r->source_lat_dms << endl;
            cout << "S_LON_DMS: " << r->source_long_dms << endl;
            cout << "-------------------------------------------" << endl;
        }
    } 

    void appendData(ofstream &db, Record<string> * r) {
        /*
            *db << r.feature_id << '|' <<
                    r.feature_name << '|' << 
                    r.feature_class << '|' <<
                    r.state_alpha << '|' <<
                    r.state_numeric << '|' <<
                    r.county_name << '|' <<
                    r.county_numeric << '|' <<
                    r.prim_lat_dms << '|' <<
                    r.prim_long_dms << '|' <<
                    r.prim_lat_dec << '|' <<
                    r.prim_long_dec << '|' <<
                    r.source_lat_dms << '|' <<
                    r.source_long_dms << '|' <<
                    r.source_lat_dec << '|' <<
                    r.source_long_dec << '|' <<
                    r.elev_in_m << '|' <<
                    r.elev_in_ft << '|' <<
                    r.map_name << '|' <<
                    r.date_created << '|' <<
                    r.date_edited << '|' <<
                    endl;
        */
        db << r->feature_id << '|' <<
                r->feature_name << '|' << 
                r->feature_class << '|' <<
                r->state_alpha << '|' <<
                r->state_numeric << '|' <<
                r->county_name << '|' <<
                r->county_numeric << '|' <<
                r->prim_lat_dms << '|' <<
                r->prim_long_dms << '|' <<
                r->prim_lat_dec << '|' <<
                r->prim_long_dec << '|' <<
                r->source_lat_dms << '|' <<
                r->source_long_dms << '|' <<
                r->source_lat_dec << '|' <<
                r->source_long_dec << '|' <<
                r->elev_in_m << '|' <<
                r->elev_in_ft << '|' <<
                r->map_name << '|' <<
                r->date_created << '|' <<
                r->date_edited << '|' <<
                endl;
    }

    void write2DB (void) {
        ofstream db(dbName, ios::app);
        cout << "Writing to DB" << endl;
        //for (vector<Record<string>>::iterator r = all.begin(); r != all.end(); ++r) {
        for (int i = 0; i < (int) all.size(); i++) {
            appendData(db, &all[i]);
        }

        db.close();
    }

    vector<Record<string>> getAllImportedRecords(void) {
        return all;
    }

    void nameIndexDebug (void) {
        nameIndex->displayData();
    }

/*
    void write2DB (GISRecord<string> * r) {
        *db << r->feature_id << '|' <<
        r->feature_name << '|' << 
        r->feature_class << '|' <<
        r->state_alpha << '|' <<
        r->state_numeric << '|' <<
        r->county_name << '|' <<
        r->county_numeric << '|' <<
        r->prim_lat_dms << '|' <<
        r->prim_long_dms << '|' <<
        r->prim_lat_dec << '|' <<
        r->prim_long_dec << '|' <<
        r->source_lat_dms << '|' <<
        r->source_long_dms << '|' <<
        r->source_lat_dec << '|' <<
        r->source_long_dec << '|' <<
        r->elev_in_m << '|' <<
        r->elev_in_ft << '|' <<
        r->map_name << '|' <<
        r->date_created << '|' <<
        r->date_edited << '|' <<
        endl;
    }
*/

};

#endif