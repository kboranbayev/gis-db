#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include "GISRecord.h"
#include "BufferPool.h"
#include "NameIndex.h"
#include "CoordinateIndex.h"

using namespace std;

class SystemManager {
    vector<Record<string>> buffer;

    World * world;

    string dbName;

    BufferPool * pool;

    NameIndex<string> *nameIndex;

    CoordinateIndex<string> * coordinateIndex;

public:
    SystemManager (string db_name, World *world_boundaries, string file, BufferPool * _pool) {
        ofstream db1(db_name.c_str(), ios::out);
        db1.close();

        if (db1.is_open())
            cerr << "It is still open even if I did close.\n";

        dbName = db_name;
        ifstream inFile(file);

        inFile.close();

        world = world_boundaries;

        buffer.resize(BUFFER_SIZE);
        nameIndex = new NameIndex<string>(INITIAL_TABLE_SIZE);

        coordinateIndex = new CoordinateIndex<string> (Point(world->w_long, world->n_lat), Point(world->e_long, world->s_lat));

        pool = _pool;
    }

    Record<string> parseStringRecord (string line) {
        Record<string> r;

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

            r = {
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
        }
        return r;
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
            }
            
        }
        return NULL;
    }

    bool inBoundary (Record<string> r) {
        return (dms2sec(r.prim_lat_dms) < world->n_lat && dms2sec(r.prim_lat_dms) > world->s_lat &&
                dms2sec(r.prim_long_dms) < world->e_long && dms2sec(r.prim_long_dms) > world->w_long);
    }

    void import (string data_file_name) {
        ifstream data(data_file_name);

        if (data.fail()) {
            cerr << "Import Error: " << data_file_name << " file not found" << endl;
            exit(2);
        }
        string line;
        long count = 0;

        while (getline(data, line)) {
            if (line.rfind("FEATURE_ID|") != 0) {
                Record<string> r = parseStringRecord (line);

                if (inBoundary(r)) {
                    writeLine2DB(line);
                    RecordIndex<string> tmp { r.feature_name, r.state_alpha, count };
                    coordinateIndex->insert(Point( dms2sec(r.prim_long_dms), dms2sec(r.prim_lat_dms)), count);
                    nameIndex->insert(tmp);
                    count++;
                }
            }
        }
    }

    string importStr (void) {
        std::ostringstream os;
        if (nameIndex->getNumFeatures() > 0) {
            os << "Imported Features by name: " << nameIndex->getNumFeatures() << endl;
            os << "Longest probe sequence: " << nameIndex->getMaxCollisions() << endl;
            os << "Imported Locations: " << nameIndex->getNumOccupied() << endl;
            os << "Average Name Length: " << nameIndex->getAverageNameLength() << endl;
        } else {
            os << "Nothing imported into Database" << endl;
            os << "Possible reasons:" << endl;
            os <<"\tb) Imported file does not contain anything or has different structure." << endl;
            os <<"\ta) Record coordinates do not lie inside the set world boundaries." << endl;
        }
        return os.str();
    }

    // Simply writes a string line into database file
    void writeLine2DB (string line) {
        ofstream db(dbName, ios::app);
        db << line << endl;
        db.close();
    }

    string worldDebug (void) {
        std::ostringstream os;
        set<long> no_duplicate_offsets;

        os << "+------------------------------------------------------------------------------------------------------------------------------------------------------+" << endl;
        for (long i = coordinateIndex->getTopLeftPoint().x; i <= coordinateIndex->getBotRightPoint().x; i+=29) {
            os << "|";
            for (long j = coordinateIndex->getTopLeftPoint().y; j > coordinateIndex->getBotRightPoint().y; j-=16) {

                long sum = 0;

                for (long x = i; x < i + 29; x++) {
                    for (long y = j; y > j - 16; y--) {
                        vector<long> t = coordinateIndex->search(Point(x,y));
                        if (t.size() != 0) {
                            for (auto & tt : t) {
                                if (!no_duplicate_offsets.count(tt)) {
                                    no_duplicate_offsets.insert(tt);
                                    sum++;
                                }
                            }
                        }
                    }
                }
                if (sum == 0) {
                    os << "  ";
                } else {
                    os << sum << " ";
                }
            }
            os << "|" << endl;
        }
        os << "+------------------------------------------------------------------------------------------------------------------------------------------------------+" << endl;
        return os.str();
    }

    Record<string> getRecordFromPool (long index) {
        Record<string> result;
        result.feature_id = -1;
        string line = pool->get(index);
        if (line == "") return result;
        result = parseStringRecord(line);
        return result;
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

        vector<long> result = coordinateIndex->search(Point(lat,lon));
        
        if (result.size() != 0) {
            os << "\tThe following feature(s) were found at (" << printDMS(longitude) << ", " << printDMS(latitude) << ")" << endl;
            for (auto & ri : result) {
                Record<string> r = getRecordFromPool(ri);
                if (r.feature_id == -1)
                    r = parseStringRecord(getDataByLine(ri));
                os << "\t" << ri << ":\t\"" << r.feature_name << "\"\t\"" << r.county_name << "\"\t\"" << r.state_alpha << "\"" << endl;
                pool->add(ri, getDataByLine(ri));
            }
        } else {
            os << "\tNothing was found at (" << printDMS(longitude) << ", " << printDMS(latitude) << ")" << endl;
        }
        return os.str();
    }


    string whatIsIn (string longitude, string latitude, string halfheight, string halfwidth) {
        std::ostringstream os;

        std::string::size_type sz;

        long lat = dms2sec(latitude);
        long lon = dms2sec(longitude);
        
        long x0 = (lat > 0) ? lat - stol (halfwidth, &sz) : lat - stol (halfwidth, &sz);
        long y0 = (lon > 0) ? lon + stol (halfheight, &sz) : lon - stol (halfheight, &sz);

        long x1 = (lat > 0) ? lat + stol (halfwidth, &sz) : lat + stol (halfwidth, &sz);
        long y1 = (lon > 0) ? lon - stol (halfheight, &sz) : lon + stol (halfheight, &sz);

        if (y0 > coordinateIndex->getTopLeftPoint().y)
            y0 = coordinateIndex->getTopLeftPoint().y;
        
        if (y1 < coordinateIndex->getBotRightPoint().y)
            y1 = coordinateIndex->getBotRightPoint().y;

        if (x0 < coordinateIndex->getTopLeftPoint().x)
            x0 = coordinateIndex->getTopLeftPoint().x;

        if (x1 > coordinateIndex->getBotRightPoint().x)
            x1 = coordinateIndex->getBotRightPoint().x;
        
        vector<long> result;

        for (long i = x0; i <= x1; i++) {
            for (long j = y0; j >= y1; j--) {
                vector<long> tmp = coordinateIndex->search(Point(i,j));
        
                if (tmp.size() != 0) {
                    for (auto & r : tmp)
                        result.push_back(r);
                }
            }
        }

        if (result.size() != 0) {
            os << "\tThe following " << result.size() << " feature(s) were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            for (auto & ri : result) {
                Record<string> r = getRecordFromPool(ri);
                if (r.feature_id == -1)
                    r = parseStringRecord(getDataByLine(ri));
                os << "\t" << ri << ":\t\"" << r.feature_name << "\"\t\"" << r.state_alpha << "\"\t\"(" << printDMS(r.prim_lat_dms) << ", " << printDMS(r.prim_long_dms) << ")\"" << endl;
                pool->add(ri, getDataByLine(ri));
            }
        } else {
            os << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
        }

        return os.str(); 
    }

    string whatIsInFilter (string longitude, string latitude, string halfheight, string halfwidth, string type) {
        std::ostringstream os;

        std::string::size_type sz;

        long lat = dms2sec(latitude);
        long lon = dms2sec(longitude);
        
        long x0 = (lat > 0) ? lat - stol (halfwidth, &sz) : lat - stol (halfwidth, &sz);
        long y0 = (lon > 0) ? lon + stol (halfheight, &sz) : lon - stol (halfheight, &sz);

        long x1 = (lat > 0) ? lat + stol (halfwidth, &sz) : lat + stol (halfwidth, &sz);
        long y1 = (lon > 0) ? lon - stol (halfheight, &sz) : lon + stol (halfheight, &sz);

        if (y0 > coordinateIndex->getTopLeftPoint().y)
            y0 = coordinateIndex->getTopLeftPoint().y;
        
        if (y1 < coordinateIndex->getBotRightPoint().y)
            y1 = coordinateIndex->getBotRightPoint().y;

        if (x0 < coordinateIndex->getTopLeftPoint().x)
            x0 = coordinateIndex->getTopLeftPoint().x;

        if (x1 > coordinateIndex->getBotRightPoint().x)
            x1 = coordinateIndex->getBotRightPoint().x;

        vector<long> result;

        for (long i = x0; i <= x1; i++) {
            for (long j = y0; j >= y1; j--) {
                vector<long> tmp = coordinateIndex->search(Point(i,j));

                if (tmp.size() != 0) {
                    for (auto & ri : tmp) {
                        Record<string> r = parseStringRecord(getDataByLine(ri));
                        if (r.type == getClassType(type))
                            result.push_back(ri);
                    }
                }

            }
        }

        if (result.size() != 0) {
            os << "\tThe following features matching your criteria were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            for (auto & ri : result) {
                Record<string> r = getRecordFromPool(ri);
                if (r.feature_id == -1)
                    r = parseStringRecord(getDataByLine(ri));
                os << "\t" << ri  << ":\t\"" << r.feature_name << "\"\t\"" << r.state_alpha << "\"\t\"(" << printDMS(r.prim_lat_dms) << ", " << printDMS(r.prim_long_dms) << ")\"" << endl;
                pool->add(ri, getDataByLine(ri));
            }
            os << "\n\tThere were " << result.size() << " features of type " << type << "." << endl;
        } else {
            os << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
        }

        return os.str();
    }

    string whatIsInLong (string longitude, string latitude, string halfheight, string halfwidth) {
        std::ostringstream os;

        std::string::size_type sz;

        long lat = dms2sec(latitude);
        long lon = dms2sec(longitude);
        
        long x0 = (lat > 0) ? lat - stol (halfwidth, &sz) : lat - stol (halfwidth, &sz);
        long y0 = (lon > 0) ? lon + stol (halfheight, &sz) : lon - stol (halfheight, &sz);

        long x1 = (lat > 0) ? lat + stol (halfwidth, &sz) : lat + stol (halfwidth, &sz);
        long y1 = (lon > 0) ? lon - stol (halfheight, &sz) : lon + stol (halfheight, &sz);

        if (y0 > coordinateIndex->getTopLeftPoint().y)
            y0 = coordinateIndex->getTopLeftPoint().y;
        
        if (y1 < coordinateIndex->getBotRightPoint().y)
            y1 = coordinateIndex->getBotRightPoint().y;

        if (x0 < coordinateIndex->getTopLeftPoint().x)
            x0 = coordinateIndex->getTopLeftPoint().x;

        if (x1 > coordinateIndex->getBotRightPoint().x)
            x1 = coordinateIndex->getBotRightPoint().x;
    
        vector<long> result;

        for (long i = x0; i <= x1; i++) {
            for (long j = y0; j >= y1; j--) {
                vector<long> tmp = coordinateIndex->search(Point(i,j));
        
                if (tmp.size() != 0) {
                    for (auto & ri : tmp)
                        result.push_back(ri);
                }
            }
        }

        if (result.size() != 0) {
            os << "\tThe following " << result.size() << " feature(s) were found in (" << printDMS(longitude) << " +/- " << halfheight << ", " << printDMS(latitude) << " +/- " << halfwidth << ")\n" << endl;
            for (auto & ri : result) {
                Record<string> r = getRecordFromPool(ri);
                if (r.feature_id == -1)
                    os << printLong(parseStringRecord(getDataByLine(ri))) << endl;
                else
                    os << printLong(r) << endl;
                pool->add(ri, getDataByLine(ri));
            }
        } else {
            os << "\tNothing was found in (\"" << printDMS(longitude) << " +/- " << halfheight << "\", \"" << printDMS(latitude) << " +/- " << halfwidth << "\")" << endl;
        }

        return os.str(); 
    }

    string whatIs (string feature, string state) {
        std::ostringstream os;
        string key = feature + "\t" + state;

        RecordIndex<string> ri = nameIndex->search(key);

        if (ri.index == -1) {
            os << "\tNo records match \"" << feature << "\"" << " and " << "\"" << state << "\"" << endl;
        } else {
            Record<string> r = getRecordFromPool(ri.index);
            if (r.feature_id == -1)
                r = parseStringRecord(getDataByLine(ri.index));
            pool->add(ri.index, getDataByLine(ri.index));
            os << "\t" << ri.index << ":\t" << r.county_name << "\t(" << printDMS(r.prim_lat_dms) << ", " << printDMS(r.prim_long_dms) << ")" << endl;
        }
        return os.str();
    }

    string str() {
        std::ostringstream os;
        return pool->str();
    }
};

#endif