#ifndef NAMEINDEX_H
#define NAMEINDEX_H

#include "GIS.h"

using namespace std;

template <typename K>
class HashFunction {
public:
    virtual unsigned int operator()(const K& key) const = 0;
    virtual ~HashFunction() = default;
};

class SimpleStringHash : public HashFunction<string> {
public:
    // Computes a simple hash value for a string
    unsigned int operator()(const string& s) const {
        // unsigned int hash = 0;
        // for (unsigned int i = 0; i < s.size(); i++) {
        //     hash += s[i];
        // }
        // BEGIN: ELF hash
        unsigned int hash = 0;
        unsigned int x = 0;
        unsigned int i = 0;
        unsigned int len = s.length();

        for (i = 0; i < len; i++)
        {
            hash = (hash << 4) + (s[i]);
            if ((x = hash & 0xF0000000) != 0)
            {
                hash ^= (x >> 24);
            }
            hash &= ~x;
        }
        return hash;
        // END: ELF hash
    }
};

/// An abstract collision resolution function
class ResolutionFunction {
public:
    virtual unsigned int operator()(int i) const = 0;
    virtual ~ResolutionFunction() = default;
};

class QuadraticProbing : public ResolutionFunction {
public:
    unsigned int operator()(int i) const {
        return (i * i + i) / 2;
    }
};

int mostSignificantBit(int x) {
    if (x == 0)
        return 0;
    int mostSignificantBit = 31;
    while ((x & (1 << mostSignificantBit)) == 0)
        mostSignificantBit--;
    return mostSignificantBit;
}

template <typename K>
class NameIndex {
    enum Status {EMPTY, OCCUPIED, DELETED};

    int c;

    int size;

    vector<K> entries;

    vector<Status> entry_status;

    vector<RecordIndex<K>> records;

    set<string> features;

    HashFunction<K> * hash;

    ResolutionFunction * f;

    int collisions;

    void expandAndRehash() {
        vector<K> new_entries, tmp_entries;
        vector<RecordIndex<K>> new_records, tmp_records;
        vector<Status> new_entry_status, tmp_entry_status;

        new_entries.resize(2 * c); // double size
        new_entry_status.resize(2 * c);
        new_records.resize(2 * c);

        tmp_entries = entries;
        tmp_entry_status = entry_status;
        tmp_records = new_records;

        entries = new_entries;
        entry_status = new_entry_status;
        records = new_records;

        collisions = 0;

        for (int i = 0; i < c; i++) {
            if (tmp_entry_status[i] == OCCUPIED) {
                // string key = "[" + records[i]record.feature_name + ":" + records[i]record.state_alpha + ", [" + std::to_string(records[i].index) + "]]";
                //insert(tmp_entries[i]);
                insert(records[i]);
            }
        }
    }

public:
    NameIndex (int n) {
        //int msb = mostSignificantBit(n);
        c = n;
        size = 0;
        hash = new SimpleStringHash();
        f = new QuadraticProbing();
        entries.resize(c);
        entry_status.resize(c);
        records.resize(c);
        collisions = 0;
    }

    ~NameIndex() {}

    bool insert (const RecordIndex<K> recordIndex) {
        string key = recordIndex.record.feature_name + "\t" + recordIndex.record.state_alpha;
        features.insert(recordIndex.record.feature_name);

        unsigned int h = hash->operator()(key) % c;
        unsigned int i = 0;
        unsigned int hi = h;
        int collision = 0;
        while (entry_status[hi] == OCCUPIED) {
            h = f->operator()(hi);
            if (entries[hi] == key) return false;
            collision++;
            if (collision > collisions) 
                collisions = collision;
            ++i;
            hi = (h + i) % c;
        }
        entry_status[hi] = OCCUPIED;
        entries[hi] = key;
        records[hi] = recordIndex;
        size++;

        if (getLoadFactor(size, c) >= 0.7) {
            expandAndRehash();
        }

        return true;
    }
/*
    bool insert (const K& key) {
        unsigned int h = hash->operator()(key) % c;
        unsigned int i = 0;
        unsigned int hi = h;
        int collision = 0;
        while (entry_status[hi] == OCCUPIED) {
            h = f->operator()(hi);
            if (entries[hi] == key) return false;
            collision++;
            if (collision > collisions) 
                collisions = collision;
            ++i;
            hi = (h + i) % c;
        }
        entry_status[hi] = OCCUPIED;
        entries[hi] = key;
        size++;

        if (getLoadFactor(size, c) >= 0.7) {
            expandAndRehash();
        }

        return true;
    }
    RecordIndex<K> searchByFeature (const) {
        unsigned int h = hash->operator()(key) % c;
        unsigned int i = 0;
        unsigned int hi = h;
    }

    RecordIndex<K> searchByState () {

    }
*/

    RecordIndex<K> * search (const K& key) {
        unsigned int h = hash->operator()(key) % c;
        unsigned int i = 0;
        unsigned int hi = h;

        while (entry_status[hi] != EMPTY) {
            if (entry_status[hi] == OCCUPIED && entries[hi] != key)
                h = f->operator()(hi);
            if (entry_status[hi] == OCCUPIED && entries[hi] == key) {
                return &records[hi];
            }
            ++i;
            hi = (h + i) % c;
        }
        return NULL;
    }

/*
    bool search (const K& key) {
        unsigned int h = hash->operator()(key) % c;
        unsigned int i = 0;
        unsigned int hi = h;

        while (entry_status[hi] != EMPTY) {
            if (entry_status[hi] == OCCUPIED && entries[hi] != key)
                h = f->operator()(hi);
            if (entry_status[hi] == OCCUPIED && entries[hi] == key) {
                return true;
            }
            ++i;
            hi = (h + i) % c;
        }
        return false;
    }
*/

    bool erase (const K& key) {
        unsigned int h = hash->operator()(key) % c;
        unsigned int i = 0;
        unsigned int hi = h;

        while (entry_status[hi] != EMPTY) {
            if (entry_status[hi] == OCCUPIED && entries[hi] == key) {
                entry_status[hi] = DELETED;
                size--;
                return true;
            }
            ++i;
            hi = (h + i) % c;
        }
        return false;
    }

    double getLoadFactor(int size, int capacity) {
        return (double) size / capacity;
    }

    string str(void) {
        std::ostringstream os;
        for (int i = 0; i < c; i++) {
            if (entry_status[i] == OCCUPIED) {
                os << "	" << i << ": " << "[" << records[i].record.feature_name << ":" << records[i].record.state_alpha << ", [" << records[i].index << "]]" << endl;
            }
        }
        return os.str();
    }

    int getTableSize(void) {
        return c;
    }

    int getAverageNameLength(void) {
        int sum = 0, count = 0;
        for (int i = 0; i < c; i++)
            if (entry_status[i] == OCCUPIED) {
                sum += entries[i].length();
                count++;
            }

        return sum / count;
    }

    int getNumFeatures(void) {
        return features.size();
    }

    int getNumRecords(void) {
        return records.size();
    }

    int getNumOccupied(void) {
        return size;
    }


    unsigned int getMaxCollisions(void) {
        return collisions;
    }

};

#endif