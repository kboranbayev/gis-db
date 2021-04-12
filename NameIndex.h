#ifndef NAMEINDEX_H
#define NAMEINDEX_H

#include "GISRecord.h"

using namespace std;

template <typename K>
class HashFunction {
public:
    virtual unsigned long operator()(const K& key) const = 0;
    virtual ~HashFunction() = default;
};

class SimpleStringHash : public HashFunction<string> {
public:
    // Computes a simple hash value for a string
    unsigned long operator()(const string& s) const {
        // BEGIN: ELF hash
        unsigned long hash = 0;
        unsigned long x = 0;
        unsigned long i = 0;
        unsigned long len = s.length();

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
    virtual unsigned long operator()(long i) const = 0;
    virtual ~ResolutionFunction() = default;
};

class QuadraticProbing : public ResolutionFunction {
public:
    unsigned long operator()(long i) const {
        return (i * i + i) / 2;
    }
};

template <typename K>
class NameIndex {
    enum Status {EMPTY, OCCUPIED, DELETED};

    long c;

    long size;

    vector<K> entries;

    vector<Status> entry_status;

    vector<RecordIndex<K>> records;

    set<string> features;

    HashFunction<K> * hash;

    ResolutionFunction * f;

    long collisions;

    void expandAndRehash() {
        vector<K> new_entries, tmp_entries;
        vector<RecordIndex<K>> new_records, tmp_records;
        vector<Status> new_entry_status, tmp_entry_status;

        new_entries.resize(2 * c); // double size
        new_entry_status.resize(2 * c);
        new_records.resize(2 * c);

        c = 2 * c;
        size = 0;

        tmp_entries = entries;
        tmp_entry_status = entry_status;
        tmp_records = records;

        entries = new_entries;
        entry_status = new_entry_status;
        records = new_records;

        for (auto & s : entry_status)
            s = EMPTY;

        for (auto & r : records)
            r.index = -1;

        collisions = 0;
        
        for (long i = 0; i < c; i++) {
            if (tmp_entry_status[i] == OCCUPIED) {
                insert(tmp_records[i]);
            }
        }
        tmp_entries.clear();
        tmp_entry_status.clear();
        tmp_records.clear();
    }

public:
    NameIndex (long n) {
        c = n;
        size = 0;
        hash = new SimpleStringHash();
        f = new QuadraticProbing();
        entries.resize(c);
        entry_status.resize(c);
        records.resize(c);

        for (auto & s : entry_status)
            s = EMPTY;

        for (auto & r : records)
            r.index = -1;
        
        collisions = 0;
    }

    ~NameIndex() {}

    bool insert (const RecordIndex<K> recordIndex) {
        string key = recordIndex.feature_name + "\t" + recordIndex.state_alpha;
        features.insert(recordIndex.feature_name);
        unsigned long h = hash->operator()(key) % c;
        unsigned long i = 0;
        unsigned long hi = h;
        long collision = 0;
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

    RecordIndex<K> search (const K& key) {
        RecordIndex<K> empty;

        empty.index = -1;
        unsigned long h = hash->operator()(key) % c;
        unsigned long i = 0;
        unsigned long hi = h;

        while (entry_status[hi] != EMPTY) {
            if (entry_status[hi] == OCCUPIED && entries[hi] != key)
                h = f->operator()(hi);
            if (entry_status[hi] == OCCUPIED && entries[hi] == key) {
                return records[hi];
            }
            ++i;
            hi = (h + i) % c;
        }
        return empty;
    }

    bool erase (const K& key) {
        unsigned long h = hash->operator()(key) % c;
        unsigned long i = 0;
        unsigned long hi = h;

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

    double getLoadFactor(long size, long capacity) {
        
        return (double) size / capacity;
    }

    string str(void) {
        std::ostringstream os;
        for (long i = 0; i < c; i++) {
            if (entry_status[i] == OCCUPIED) {
                os << "	" << i << ": " << "[" << records[i].feature_name << ":" << records[i].state_alpha << ", [" << records[i].index << "]]" << endl;
            }
        }
        return os.str();
    }

    long getTableSize(void) {
        return c;
    }

    long getAverageNameLength(void) {
        long sum = 0, count = 0;
        for (long i = 0; i < c; i++)
            if (entry_status[i] == OCCUPIED) {
                sum += entries[i].length();
                count++;
            }

        return sum / count;
    }

    long getNumFeatures(void) {
        return features.size();
    }

    long getNumRecords(void) {
        return records.size();
    }

    long getNumOccupied(void) {
        return size;
    }


    unsigned long getMaxCollisions(void) {
        return collisions;
    }

};

#endif