#ifndef COORDINATEINDEX_H
#define COORDINATEINDEX_H

#include "GIS.h"

#define N 4

using namespace std;

struct Point {
    long x; // long
    long y; // lat
    Point(long _x, long _y) {
        x = _x;
        y = _y;
    }
    Point() {
        x = 0;
        y = 0;
    }

    string str() {
        std::ostringstream os;
        os << "(" << y << "," << x << ")";
        return os.str();
    }
};

template <typename K>
struct Node {
    Point                   topLeft,
                            botRight; // Node region boundaries
    // vector<Record <K>>      records;        // Holds a collection of records up to N
    vector<Point>           coordinates;    // Holds corresponding coordinate points
    vector<RecordIndex<K>>      records;
    // vector<int>                 lineNums;       // Holds corresponding file line numbers
    bool                    isFull;         // True if bucket reached its limit to N, False otherwise
    bool                    isLeaf;         // True if Node is a leaf and accepts more data
                                            // False if Node is split into 4 inner nodes
    Node () {
        isFull = false;
        isLeaf = true;
    }

    Node (Point _topLeft, Point _botRight) {
        topLeft = _topLeft;
        botRight = _botRight;
        isFull = false;
        isLeaf = true;
        nw = NULL;
        ne = NULL;
        sw = NULL;
        se = NULL;
    }

    // void add (Point _p, int _lineNo, Record <K> _record) {
    void add (Point _p, RecordIndex <K> _record) {
        //cout << "\tADD: " << _p.str() << " SIZE: " << coordinates.size() << endl;
        if (coordinates.size() < N || !isFull) {
            coordinates.push_back(_p);
            records.push_back(_record);
            if (coordinates.size() == N) isFull = true;
        }
    }

    string str () {
        std::ostringstream os;

        for (unsigned i = 0; i < coordinates.size(); i++) {
            os << "[" << coordinates[i].str() << ", " << records[i].index << "] ";
        }
        os << endl;

        return os.str();
    }

    Node * nw;
    Node * ne;
    Node * sw;
    Node * se;
};

template <typename K>
class CoordinateIndex {
    private:
        Node<K> * root;
        Point topLeft, botRight;
    
    public:
        CoordinateIndex (Point _topLeft, Point _botRight) {
            topLeft = _topLeft;
            botRight = _botRight;
            root = NULL;
        }

        string str () {
            std::ostringstream os;
            
            if (root == NULL) return "";

            if (root != NULL) {
                if (root->isLeaf) {
                    os << root->str() << "\t";
                }
                
                if (!root->isLeaf) {
                    os << "@" << endl << "\t\t";
                    Node<K> * tmp = root;
                    if (root->nw != NULL) {
                        root = root->nw;
                        os << "\t" << str();    
                    } else {
                        os << "\t*" << endl;
                    }
                    root = tmp;
                    if (root->ne != NULL) {
                        root = root->ne;
                        os << "\t" << str(); 
                    } else {
                        os << "\t*" << endl;
                    }
                    root = tmp;
                    if (root->sw != NULL) {
                        root = root->sw;
                        os << "\t" << str();    
                    } else {
                        os << "\t*" << endl;
                    }
                    root = tmp;
                    if (root->se != NULL) {
                        root = root->se;
                        os << "\t" << str();    
                    } else {
                        os << "\t*" << endl;
                    }
                    
                    root = tmp;
                }
            }

            return os.str();
        } 

        void subdivide () {
            long newX = (root->topLeft.x + root->botRight.x) / 2;
            long newY = (root->topLeft.y + root->botRight.y) / 2;

            root->isLeaf = false;

            Node<K> * tmp = root;
            for (unsigned i = 0; i < tmp->coordinates.size(); i++) {
                if (tmp->topLeft.x <= tmp->coordinates[i].x && tmp->coordinates[i].x <= newX)
                {
                    // Indicates topLeftTree
                    if (tmp->topLeft.y >= tmp->coordinates[i].y && tmp->coordinates[i].y >= newY) {
                        if (root->nw == NULL) {
                            
                            root->nw = new Node<K> (Point(tmp->topLeft.x, tmp->topLeft.y), Point(newX, newY));
                        }
                        root = root->nw;
                        insert(tmp->coordinates[i], tmp->records[i]);
                    } else {
                        if (root->sw == NULL) {

                            root->sw = new Node<K> (Point(tmp->topLeft.x, newY), Point(newX, tmp->botRight.y));
                        }
                        root = root->sw;
                        insert(tmp->coordinates[i], tmp->records[i]);
                    }
                } else {
                    if (tmp->topLeft.y >= tmp->coordinates[i].y && tmp->coordinates[i].y >= newY) {
                        if (root->ne == NULL) {
                            root->ne = new Node<K> (Point(newX, tmp->topLeft.y), Point(tmp->botRight.x, newY));
                        }
                        root = root->ne;
                        insert(tmp->coordinates[i], tmp->records[i]);
                    } else {
                        if (root->se == NULL) {
                            root->se = new Node<K> (Point(newX, newY), Point(tmp->botRight.x, tmp->botRight.y));
                        }
                        root = root->se;

                        insert(tmp->coordinates[i], tmp->records[i]);
                    }
                }   
                root = tmp;
            }
        }
        

        // void insert (Point _p, int _lineNo, Record<K> _record) {
        void insert (Point _p, RecordIndex<K> _record) {
            if (!inBoundary(_p)) {
                cerr << "Not in World boundary" << endl;
                return;
            }

            if (root == NULL) {
                root = new Node<K> (topLeft, botRight);
                root->add(_p, _record);
                return;
            }

            if (root != NULL) {
                if (root->isFull && root->isLeaf) { // Subdivide
                    subdivide();
                    insert(_p, _record);
                    return;
                }

                if (!root->isFull && root->isLeaf) {
                    root->add(_p, _record);
                    return;
                }

                if (!root->isLeaf) {
                    long newX = (root->topLeft.x + root->botRight.x) / 2;
                    long newY = (root->topLeft.y + root->botRight.y) / 2;
                    Node<K> * tmp = root;
                    if (root->topLeft.x <= _p.x && _p.x <= newX)
                    {
                        // Indicates topLeftTree
                        if (root->topLeft.y >= _p.y && _p.y >= newY) {
                            if (root->nw == NULL) {
                                root->nw = new Node<K> (Point(root->topLeft.x, root->topLeft.y), Point(newX, newY));
                            }
                            root = root->nw;
                            insert(_p, _record);
                        } else {
                            if (root->sw == NULL) {
                                root->sw = new Node<K> (Point(root->topLeft.x, newY), Point(newX, root->botRight.y));
                            }
                            root = root->sw;
                            insert(_p, _record);
                        }
                    } else {
                        if (root->topLeft.y >= _p.y && _p.y >= newY) {
                            if (root->ne == NULL) {
                                root->ne = new Node<K> (Point(newX, root->topLeft.y), Point(root->botRight.x, newY));
                            }
                            root = root->ne;
                            insert(_p, _record);
                        } else {
                            if (root->se == NULL) {
                                root->se = new Node<K> (Point(newX, newY), Point(root->botRight.x, root->botRight.y));
                            }
                            root = root->se;
                            insert(_p, _record);
                        }
                    }
                    root = tmp;
                }
            }
        }

        vector<RecordIndex<K>> search (Point _p) {
            vector<RecordIndex<K>> result;

            if (!inBoundary(_p)) {
                cerr << "Not in World boundary" << endl;
                return {};
            }

            if (root == NULL) {
                return {};
            }

            if (root != NULL) {
                if (root->isLeaf) {
                    for (unsigned i = 0; i < root->coordinates.size(); i++) {
                        if (root->coordinates[i].x == _p.x && root->coordinates[i].y == _p.y)
                            result.push_back(root->records[i]);
                    }
                    if (result.size() > 0) return result;
                }

                if (!root->isLeaf || result.size() == 0) {
                    Node<K> * tmp = root;
                    long newX = (root->topLeft.x + root->botRight.x) / 2;
                    long newY = (root->topLeft.y + root->botRight.y) / 2;

                    if (root->topLeft.x <= _p.x && _p.x <= newX)
                    {
                        if (root->topLeft.y >= _p.y && _p.y >= newY) {
                            if (root->nw == NULL) {
                                return {};
                            }
                            root = root->nw;
                            result = search(_p);
                        } else {
                            if (root->sw == NULL) {
                                return {};
                            }
                            root = root->sw;
                            result = search(_p);
                        }
                    } else {
                        if (root->topLeft.y >= _p.y && _p.y >= newY) {
                            if (root->ne == NULL) {
                                return {};
                            }
                            root = root->ne;
                            result = search(_p);
                        } else {
                            if (root->se == NULL) {
                                return {};
                            }
                            root = root->se;
                            result = search(_p);
                        }
                    }
                    root = tmp;
                }
            }
            return result;
        }

        bool inBoundary (Point _p) {
            return (_p.x >= topLeft.x &&
                    _p.x <= botRight.x &&
                    _p.y <= topLeft.y &&
                    _p.y >= botRight.y);
        }
};

#endif

