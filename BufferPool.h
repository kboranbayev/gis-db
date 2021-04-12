#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include "GISRecord.h"

using namespace std;

typedef struct MemoryPage {
    MemoryPage *prev, *next;
    string data;
    unsigned key;
} MemoryPage;

typedef struct Memory {
    MemoryPage *head, *tail;
} Memory;

typedef map<unsigned, MemoryPage *> CacheIndex;

typedef struct Cache {
    unsigned capacity;
    Memory memory;
    CacheIndex index;
} Cache;

class BufferPool {
    private:
        Cache cache;
        unsigned capacity;

        bool full() {
            return cache.index.size() == capacity;
        }

        bool empty() {
            return cache.index.size() == 0;
        }

        MemoryPage * newMemoryPage (unsigned _key, string _data) {
            MemoryPage * newPage = new MemoryPage;
            newPage->data = _data;
            newPage->key = _key;

            newPage->prev = newPage->next = NULL;

            return newPage;
        }

        void makeARoom() {
            if (!full()) return;

            if (cache.memory.head == cache.memory.tail)
                cache.memory.head = NULL;
            
            MemoryPage * pageToBeRemoved = cache.memory.tail;
            cache.memory.tail = pageToBeRemoved->prev;

            if (cache.memory.tail != NULL)
                cache.memory.tail->next = NULL;

            cache.index.erase(pageToBeRemoved->key);
            delete pageToBeRemoved;
        }

        void dump() {
            MemoryPage *memoryPage = cache.memory.head;

            while (memoryPage != NULL) {
                memoryPage = memoryPage->next;
            }
        }

        void dumpIndex() {

            for (CacheIndex::iterator indexEntry = cache.index.begin();
                indexEntry != cache.index.end(); ++indexEntry) {}
        }
    public:
        BufferPool(unsigned _capacity) {
            capacity = _capacity;
            build(capacity);
        }

        void build(unsigned n) {
            cache.capacity = n;

            for (CacheIndex::iterator indexEntry = cache.index.begin();
                indexEntry != cache.index.end(); ++indexEntry) {
                cache.index.erase(indexEntry->first);
                delete (indexEntry->second);
            }

            cache.memory.head = cache.memory.tail = NULL;
            dump();
            dumpIndex();
        }

        void destruct() {
            cout << "Destructing LRU Cache along with all data related to it..." << endl;
            build(0);
        }

        void add(unsigned key, string data) {
            CacheIndex::iterator indexEntry = cache.index.find(key);

            if (indexEntry != cache.index.end()) {
                bool isHead = false;
                bool isTail = false;

                if (cache.memory.head == indexEntry->second) {
                    isHead = true;

                    cache.memory.head = indexEntry->second->next;

                    if (cache.memory.head != NULL) {
                        cache.memory.head->prev = NULL;
                    }
                }

                if (cache.memory.tail == indexEntry->second) {
                    isTail = true;
                    cache.memory.tail = indexEntry->second->prev;

                    if (cache.memory.tail != NULL) {
                        cache.memory.tail->next = NULL;
                    }
                }

                if (!isHead && !isTail) {
                    indexEntry->second->prev->next = indexEntry->second->next;
                    indexEntry->second->next->prev = indexEntry->second->prev;
                }

                delete (indexEntry->second);
            } else if (full()) {
                makeARoom();
            }

            MemoryPage *newPage = newMemoryPage(key, data);
            newPage->next = cache.memory.head;

            if (empty()) {
                cache.memory.head = cache.memory.tail = newPage;
            } else {
                cache.memory.head->prev = newPage;
                cache.memory.head = newPage;
            }

            cache.index[key] = newPage;
            dump();
            dumpIndex();
        }

        string get(unsigned key) {
            CacheIndex::iterator indexEntry = cache.index.find(key);

            if (indexEntry != cache.index.end()) {
                MemoryPage *foundMemoryPage = indexEntry->second;
                if (foundMemoryPage !=
                    cache.memory.head) {
                    foundMemoryPage->prev->next = foundMemoryPage->next;
                    if (foundMemoryPage->next)
                        foundMemoryPage->next->prev = foundMemoryPage->prev;

                    if (foundMemoryPage == cache.memory.tail) {
                        cache.memory.tail = foundMemoryPage->prev;
                        cache.memory.tail->next = NULL;
                    }

                    foundMemoryPage->next = cache.memory.head;
                    foundMemoryPage->prev = NULL;

                    foundMemoryPage->next->prev = foundMemoryPage;

                    cache.memory.head = foundMemoryPage;

                    dump();
                    dumpIndex();
                }
                return foundMemoryPage->data;
            }
            return "";
        }
    
        string str() {
            std::ostringstream os;
            MemoryPage *tmp = cache.memory.head;
            os << "MRU" << endl;

            while (tmp != NULL) {
                os << "\t" << tmp->key << ": " << tmp->data << endl;
                tmp = tmp->next;
            }
            os << "LRU" << endl;

            return os.str();
        }
};

#endif