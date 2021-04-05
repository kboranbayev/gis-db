#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include "GIS.h"
#include "BufferPool.h"
#include "NameIndex.h"
#include "CoordinateIndex.h"

using namespace std;

class SystemManager {
    int size;

    vector<Record<string>> buffer;

    World * world;

    string dbName;

    BufferPool * pool;

    NameIndex<string> *nameIndex;

    CoordinateIndex<string> * coordinateIndex;

public:
    SystemManager (string db_name, World *world_boundaries, string file) {
        ofstream db1(db_name.c_str(), ios::out);
        db1.close();

        if (db1.is_open())
            cout << "It is still open even if I did close.\n";

        dbName = db_name;
        ifstream inFile(file); 
        
        int n = count(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>(), '\n');

        size = n;

        inFile.close();

        world = world_boundaries;

        buffer.resize(BUFFER_SIZE);
        nameIndex = new NameIndex<string>(INITIAL_TABLE_SIZE);

        coordinateIndex = new CoordinateIndex<string> (Point(world->w_long, world->n_lat), Point(world->e_long, world->s_lat));

        pool = new BufferPool(BUFFER_SIZE);
    }

    int getSize(void) {
        return size;
    }

    string getDataByLine (int lineNo) {
        ifstream data(dbName);
        if (data.fail()) {
            cerr << "getDataByLine Error: file not found" << endl;
            exit(2);
        }

        string line;
        int count = 0;

        while (getline(data, line)) {
            ++count;
            if (count == lineNo + 1) {
                return line;
                /*
                    istringstream iss(line);
                    string feature_id, feature_name, feature_class, state_alpha, state_numeric, county_name, county_numeric;
                    string prim_lat_dms, prim_long_dms, prim_lat_dec, prim_long_dec;
                    string source_lat_dms, source_long_dms, source_lat_dec, source_long_dec;
                    string elec_in_m, elec_in_ft, map_name;
                    string date_created, date_edited;

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
                        atol(prim_lat_dec.c_str()),
                        atol(prim_long_dec.c_str()),
                        source_lat_dms,
                        source_long_dms,
                        atol(source_lat_dec.c_str()),
                        atol(source_long_dec.c_str()),
                        elec_in_m,
                        elec_in_ft,
                        map_name,
                        date_created,
                        date_edited 
                    };

                    if (dms2sec(prim_lat_dms) < world->n_lat && dms2sec(prim_lat_dms) > world->s_lat && 
                        dms2sec(prim_long_dms) < world->e_long && dms2sec(prim_long_dms) > world->w_long) {
                        
                        buffer.push_back(r);
                    }

                    cout << "F_ID: " << feature_id << endl;
                    cout << "F_N: " << feature_name << endl;
                    cout << "F_C: " << feature_class << endl;
                    cout << "S_A: " << state_alpha << endl;
                    cout << "S_N: " << state_numeric << endl;
                    cout << "C_NA: " << county_name << endl;
                    cout << "C_NU: " << county_numeric << endl;

                    cout << "S_LAT_DMS: " << source_lat_dms << endl;
                    cout << "S_LON_DMS: " << source_long_dms << endl;
                */
            }
            
        }
        return NULL;
    }


    string import (string data_file_name) {
        std::ostringstream os;

        ifstream data(data_file_name);

        if (data.fail()) {
            cerr << "Import Error: " << data_file_name << " file not found" << endl;
            exit(2);
        }
        string line;
        int count = 0;

        while (getline(data, line)) {
            istringstream iss(line);
            string feature_id, feature_name, feature_class, state_alpha, state_numeric, county_name, county_numeric;
            string prim_lat_dms, prim_long_dms, prim_lat_dec, prim_long_dec;
            string source_lat_dms, source_long_dms, source_lat_dec, source_long_dec;
            string elec_in_m, elec_in_ft, map_name;
            string date_created, date_edited;
            ClassType type;
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
                getline(iss, elec_in_m, '|');
                getline(iss, elec_in_ft, '|');
                getline(iss, map_name, '|');
                getline(iss, date_created, '|');
                getline(iss, date_edited, '|');

                type = setClassType(feature_class);

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
                    atol(prim_lat_dec.c_str()),
                    atol(prim_long_dec.c_str()),
                    source_lat_dms,
                    source_long_dms,
                    atol(source_lat_dec.c_str()),
                    atol(source_long_dec.c_str()),
                    elec_in_m,
                    elec_in_ft,
                    map_name,
                    date_created,
                    date_edited,
                    type
                };

                if (dms2sec(prim_lat_dms) < world->n_lat && dms2sec(prim_lat_dms) > world->s_lat && 
                    dms2sec(prim_long_dms) < world->e_long && dms2sec(prim_long_dms) > world->w_long) {
                    writeLine2DB(line);
                    RecordIndex<string> tmp { r, count };
                    coordinateIndex->insert(Point( dms2sec(prim_long_dms), dms2sec(prim_lat_dms)), tmp);
                    nameIndex->insert(tmp);
                    count++;
                }
            }       
        }
        os << "Imported Features by name: " << nameIndex->getNumFeatures() << endl;
        os << "Longest probe sequence: " << nameIndex->getMaxCollisions() << endl;
        os << "Imported Locations: " << nameIndex->getNumOccupied() << endl;
        os << "Average Name Length: " << nameIndex->getAverageNameLength() << endl;
        return os.str();
    }

    void writeLine2DB (string line) {
        ofstream db(dbName, ios::app);
        db << line << endl;
        db.close();
    }

    string hashDebug (void) {
        std::ostringstream os;
        os << "Format display is " << endl;
        os << "Slot number: " << "data record" << endl;
        os << "Current table size is " << nameIndex->getTableSize() << endl;
        os << "Number of elements in table is " << nameIndex->getNumOccupied() << endl << endl;
        
        os << nameIndex->str();
        return os.str();
    }

    string quadDebug (void) {
        std::ostringstream os;

        os << coordinateIndex->str();
        return os.str();
    }

    string whatIsAt (string longitude, string latitude) {
        std::ostringstream os;
        long lat = dms2sec(latitude);
        long lon = dms2sec(longitude);

        vector<RecordIndex<string>> result = coordinateIndex->search(Point(lat,lon));
        
        if (result.size() != 0) {
            os << "\tThe following feature(s) were found at (" << printDMS(longitude) << ", " << printDMS(latitude) << ")" << endl;
            cout << "\tThe following feature(s) were found at (" << printDMS(longitude) << ", " << printDMS(latitude) << ")" << endl;
            for (auto & r : result) {
                os << "\t" << r.index << ":\t\"" << r.record.feature_name << "\"\t\"" << r.record.county_name << "\"\t\"" << r.record.state_alpha << "\"" << endl;
                cout << "\t" << r.index << ":\t\"" << r.record.feature_name << "\"\t\"" << r.record.county_name << "\"\t\"" << r.record.state_alpha << "\"" << endl;
                pool->add(r.index, getDataByLine(r.index));
            }
        } else {
            os << "\tNothing was found at (" << printDMS(longitude) << ", " << printDMS(latitude) << ")" << endl;
            cout << "\tNothing was found at (" << printDMS(longitude) << ", " << printDMS(latitude) << ")" << endl;
        }
        return os.str();
    }


    string whatIsIn (string longitude, string latitude, string halfheight, string halfwidth) {
        std::ostringstream os;

        std::string::size_type sz;

        long lat = dms2sec(latitude);
        long lon = dms2sec(longitude);
        
        long x0 = (lon > 0) ? lon - stol (halfheight, &sz) : lon + stol (halfheight, &sz);
        long y0 = (lat > 0) ? lat + stol (halfwidth, &sz) : lat + stol (halfwidth, &sz);

        long x1 = (lon > 0) ? lon + stol (halfheight, &sz) : lon - stol (halfheight, &sz);
        long y1 = (lat > 0) ? lat - stol (halfwidth, &sz) : lat - stol (halfwidth, &sz);
    
        vector<RecordIndex<string>> result;

        for (long i = x0; i <= x1; i++) {
            for (long j = y0; j >= y1; j--) {
                vector<RecordIndex<string>> tmp = coordinateIndex->search(Point(j,i));
        
                if (tmp.size() != 0) {
                    for (auto & r : tmp)
                        result.push_back(r);
                }
            }
        }

        if (result.size() != 0) {
            os << "\tThe following " << result.size() << " feature(s) were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            cout << "\tThe following " << result.size() << " feature(s) were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            for (auto & r : result) {
                os << "\t" << r.index << ":\t\"" << r.record.feature_name << "\"\t\"" << r.record.state_alpha << "\"\t\"(" << printDMS(r.record.prim_lat_dms) << ", " << printDMS(r.record.prim_long_dms) << ")\"" << endl;
                cout << "\t" << r.index << ":\t\"" << r.record.feature_name << "\"\t\"" << r.record.state_alpha << "\"\t\"(" << printDMS(r.record.prim_lat_dms) << ", " << printDMS(r.record.prim_long_dms) << ")\"" << endl;
                pool->add(r.index, getDataByLine(r.index));
            }
        } else {
            os << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
            cout << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
        }

        return os.str(); 
    }

    string whatIsInFilter (string longitude, string latitude, string halfheight, string halfwidth, string type) {
        std::ostringstream os;

        std::string::size_type sz;

        long lat = dms2sec(latitude);
        long lon = dms2sec(longitude);
        
        long x0 = (lon > 0) ? lon - stol (halfheight, &sz) : lon + stol (halfheight, &sz);
        long y0 = (lat > 0) ? lat + stol (halfwidth, &sz) : lat + stol (halfwidth, &sz);

        long x1 = (lon > 0) ? lon + stol (halfheight, &sz) : lon - stol (halfheight, &sz);
        long y1 = (lat > 0) ? lat - stol (halfwidth, &sz) : lat - stol (halfwidth, &sz);
    
        vector<RecordIndex<string>> result;

        for (long i = x0; i <= x1; i++) {
            for (long j = y0; j >= y1; j--) {
                vector<RecordIndex<string>> tmp = coordinateIndex->search(Point(j,i));
        
                if (tmp.size() != 0) {
                    for (auto & r : tmp)
                        if (r.record.type == getClassType(type))
                            result.push_back(r);
                }
            }
        }

        if (result.size() != 0) {
            os << "\tThe following features matching your criteria were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            cout << "\tThe following features matching your criteria were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            for (auto & r : result) {
                os << "\t" << r.index << ":\t\"" << r.record.feature_name << "\"\t\"" << r.record.state_alpha << "\"\t\"(" << printDMS(r.record.prim_lat_dms) << ", " << printDMS(r.record.prim_long_dms) << ")\"" << endl;
                cout << "\t" << r.index << ":\t\"" << r.record.feature_name << "\"\t\"" << r.record.state_alpha << "\"\t\"(" << printDMS(r.record.prim_lat_dms) << ", " << printDMS(r.record.prim_long_dms) << ")\"" << endl;
                pool->add(r.index, getDataByLine(r.index));
            }
            os << "\n\tThere were " << result.size() << " features of type " << type << "." << endl;
            cout << "\n\tThere were " << result.size() << " features of type " << type << "." << endl;
        } else {
            os << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
            cout << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
        }

        return os.str();
    }

    string whatIsInLong (string longitude, string latitude, string halfheight, string halfwidth) {
        std::ostringstream os;

        std::string::size_type sz;

        long lat = dms2sec(latitude);
        long lon = dms2sec(longitude);
        
        long x0 = (lon > 0) ? lon - stol (halfheight, &sz) : lon + stol (halfheight, &sz);
        long y0 = (lat > 0) ? lat + stol (halfwidth, &sz) : lat + stol (halfwidth, &sz);

        long x1 = (lon > 0) ? lon + stol (halfheight, &sz) : lon - stol (halfheight, &sz);
        long y1 = (lat > 0) ? lat - stol (halfwidth, &sz) : lat - stol (halfwidth, &sz);
    
        vector<RecordIndex<string>> result;

        for (long i = x0; i <= x1; i++) {
            for (long j = y0; j >= y1; j--) {
                vector<RecordIndex<string>> tmp = coordinateIndex->search(Point(j,i));
        
                if (tmp.size() != 0) {
                    for (auto & r : tmp)
                        result.push_back(r);
                }
            }
        }

        if (result.size() != 0) {
            os << "\tThe following " << result.size() << " feature(s) were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            cout << "\tThe following " << result.size() << " feature(s) were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            for (auto & r : result) {
                os << printLong(r.record) << endl;
                pool->add(r.index, getDataByLine(r.index));
            }
        } else {
            os << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
            cout << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
        }

        return os.str(); 
    }

    string whatIs (string feature, string state) {
        std::ostringstream os;
        string key = feature + "\t" + state;

        RecordIndex<string> * ri = nameIndex->search(key);
        if (ri == NULL) {
            os << "\tNo records match \"" << feature << "\"" << " and " << "\"" << state << "\"" << endl;
            cout << "\tNo records match \"" << feature << "\"" << " and " << "\"" << state << "\"" << endl;
        } else {
            pool->add(ri->index, getDataByLine(ri->index));
            os << "\t" << ri->index << ":\t" << ri->record.county_name << "\t(" << printDMS(ri->record.prim_lat_dms) << ", " << printDMS(ri->record.prim_long_dms) << ")" << endl;
            cout << "\t" << ri->index << ":\t" << ri->record.county_name << "\t(" << printDMS(ri->record.prim_lat_dms) << ", " << printDMS(ri->record.prim_long_dms) << ")" << endl;
        }
        return os.str();
    }

    string str() {
        std::ostringstream os;
        cout << pool->str() << endl; 
        return pool->str();
    }
};

#endif