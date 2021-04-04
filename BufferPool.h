#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include "GIS.h"

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
            
            cout << "Cache is full. Removing key " << pageToBeRemoved->key << " from cache before insertint a new one..." << endl;

            cache.index.erase(pageToBeRemoved->key);
            delete pageToBeRemoved;
        }

        void dump() {
            // cout << "Cache's memory state: " << "[ ";

            MemoryPage *memoryPage = cache.memory.head;

            while (memoryPage != NULL) {
                //cout << memoryPage->key << " => " << memoryPage->data << ", ";
                memoryPage = memoryPage->next;
            }

            //cout << "]" << endl;
        }

        void dumpIndex() {
            //cout << "Cache's index state: " << "[  ";

            for (CacheIndex::iterator indexEntry = cache.index.begin();
                indexEntry != cache.index.end(); ++indexEntry) {}
                //cout << indexEntry->first << " => " << indexEntry->second << ",  ";

            //cout << "]" << endl;
        }
    public:
        BufferPool(unsigned _capacity) {
            capacity = _capacity;
            build(capacity);
        }

        void build(unsigned n) {
            // if (n > 0)
            //     // cout << "Building a LRU Cache with capacity for " << n << " entries..." << endl;
            // else
                // cout << "Setting cache into a new empty state..." << endl;

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
            //cout << "Adding value '" << data << "' to key: " << key << endl;

            CacheIndex::iterator indexEntry = cache.index.find(key);

            if (indexEntry != cache.index.end()) {  // If cache has already an entry for the given key, then purges it from cache's memory first
                //cout << "Key " << key << " has already an entry in the cache. Replacing it..." << endl;

                //cache.index.erase(key);

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
            } else if (full()) { // If cache has not the entry fro the given key yet, and it is full, then makes a room by removing the page at last position in cache's memory
                makeARoom();
            }

            // Creates a new memory page node with given key and data,
            // and adds it to the first position in cache's memory
            MemoryPage *newPage = newMemoryPage(key, data);
            newPage->next = cache.memory.head;

            if (empty()) {  // If queue is empty, makes both pointers 'memory.head' and 'memory.tail' to point to the address of the newly created page
                cache.memory.head = cache.memory.tail = newPage;
            } else { // Otherwise, just makes 'memory.head' pointing to the new page and updates the property 'prev' of the page that was at memory head
                cache.memory.head->prev = newPage;
                cache.memory.head = newPage;
            }

            // Inserts the new page entry into cache's index container
            cache.index[key] = newPage;
            dump();
            dumpIndex();
        }

        string get(unsigned key) {
            cout << "Reading value from key: " << key << endl;

            CacheIndex::iterator indexEntry = cache.index.find(key);

            if (indexEntry != cache.index.end()) {
                MemoryPage *foundMemoryPage = indexEntry->second;
                if (foundMemoryPage !=
                    cache.memory.head) { // The data for the key informed is there, but the page carrying it is not at the first position of cache's memory. So, updates it
                    // Unties the requested page from its current location in cache's memory.
                    foundMemoryPage->prev->next = foundMemoryPage->next;
                    if (foundMemoryPage->next)
                        foundMemoryPage->next->prev = foundMemoryPage->prev;

                    // If the requested page is at last position in cache's memory, then updates 'memory.tail' pointer as long as it is going to be moved to the first position now
                    if (foundMemoryPage == cache.memory.tail) {
                        cache.memory.tail = foundMemoryPage->prev;
                        cache.memory.tail->next = NULL;
                    }

                    // Places the requested page before current first page in cache's memory
                    foundMemoryPage->next = cache.memory.head;
                    foundMemoryPage->prev = NULL;

                    // Makes 'prev' pointer of former first page pointing to the new first page
                    foundMemoryPage->next->prev = foundMemoryPage;

                    // Updates 'memory.head' pointer to the requested page's address
                    cache.memory.head = foundMemoryPage;

                    dump();
                    dumpIndex();
                }
                return foundMemoryPage->data;
            } else {
                cerr << "Cache page fault: Key " << key << " does not exist in cache!" << endl;
                throw std::invalid_argument("Unknown key!");
            }
        }
    
        string str() {
            std::ostringstream os;
            MemoryPage *tmp = cache.memory.head;
            os << "MRU" << endl;

            // setting starting position
            // while (tmp->prev != NULL) {
            //     tmp = tmp->prev;
            // }

            while (tmp != NULL) {
                os << "\t" << tmp->key << ": " << tmp->data << endl;
                tmp = tmp->next;
            }
            os << "LRU" << endl;

            return os.str();
        }
};

#endif