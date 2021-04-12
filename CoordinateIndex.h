#ifndef COORDINATEINDEX_H
#define COORDINATEINDEX_H

#include "GISRecord.h"

#define     N   4       // Number of objects that can be stored in a single node

using namespace std;

struct Point {
    long x;     // lat
    long y;     // long
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
                            botRight;       // Node region boundaries
    vector<Point>           coordinates;    // Holds corresponding coordinate points
    vector<long>            record_offsets;
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

    void add (Point _p, long _offset) {
        if (coordinates.size() < N || !isFull) {
            coordinates.push_back(_p);
            record_offsets.push_back(_offset);
            if (coordinates.size() == N) isFull = true;
        }
    }

    string str () {
        std::ostringstream os;

        for (unsigned i = 0; i < coordinates.size(); i++) {
            os << "[" << coordinates[i].str() << ", " << record_offsets[i] << "] ";
        }

        return os.str();
    }

    Node * nw;      // North-West
    Node * ne;      // North-East 
    Node * sw;      // South-West
    Node * se;      // South-East
};

template <typename K>
class CoordinateIndex {
    private:
        Node<K> * root;
    
    public:
        Point topLeft, botRight;
        CoordinateIndex (Point _topLeft, Point _botRight) {
            topLeft = _topLeft;
            botRight = _botRight;
            root = NULL;
        }

        Point getTopLeftPoint (void) {
            return root->topLeft;
        }

        Point getBotRightPoint (void) {
            return root->botRight;
        }

        vector<long> searchNW (Point _topLeft, Point _botRight) {
            vector<long> result;

            return result;
        }

        string str () {
            std::ostringstream os;
            
            if (root == NULL) return "*\n";
 
            if (root != NULL) {
                Node<K> * tmp = root;
                if (root->isLeaf) {
                    os << "  " << root->str() << endl;
                }

                if (!root->isLeaf) {
                    root = root->sw;
                    os << "  " << str();
                    root = tmp;
                    
                    root = root->nw;
                    os << "  " << str();
                    root = tmp;
                    
                    os << "@" << endl;

                    root = root->se;
                    os << "  " << str();
                    root = tmp;

                    root = root->ne;
                    os << "  " << str();
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
                    if (tmp->topLeft.y >= tmp->coordinates[i].y && tmp->coordinates[i].y >= newY) {
                        if (root->nw == NULL) {
                            
                            root->nw = new Node<K> (Point(tmp->topLeft.x, tmp->topLeft.y), Point(newX, newY));
                        }
                        root = root->nw;
                        insert(tmp->coordinates[i], tmp->record_offsets[i]);
                    } else {
                        if (root->sw == NULL) {

                            root->sw = new Node<K> (Point(tmp->topLeft.x, newY), Point(newX, tmp->botRight.y));
                        }
                        root = root->sw;
                        insert(tmp->coordinates[i], tmp->record_offsets[i]);
                    }
                } else {
                    if (tmp->topLeft.y >= tmp->coordinates[i].y && tmp->coordinates[i].y >= newY) {
                        if (root->ne == NULL) {
                            root->ne = new Node<K> (Point(newX, tmp->topLeft.y), Point(tmp->botRight.x, newY));
                        }
                        root = root->ne;
                        insert(tmp->coordinates[i], tmp->record_offsets[i]);
                    } else {
                        if (root->se == NULL) {
                            root->se = new Node<K> (Point(newX, newY), Point(tmp->botRight.x, tmp->botRight.y));
                        }
                        root = root->se;
                        insert(tmp->coordinates[i], tmp->record_offsets[i]);
                    }
                }   
                root = tmp;
            }

            root->coordinates.clear();
            root->record_offsets.clear();
        }

        void insert (Point _p, long _offset) {
            if (!inBoundary(_p)) {
                // cerr << "Out of bound" << endl; // uncomment this line, to check the import function
                return;
            }

            if (root == NULL) {
                root = new Node<K> (topLeft, botRight);
                root->add(_p, _offset);
                return;
            }

            if (root != NULL) {
                if (root->isFull && root->isLeaf) { // Subdivide
                    int count_identicals = 0;
                    for (auto & r : root->coordinates) {
                        if (_p.x == r.x && _p.y == r.y)
                            count_identicals++;
                        if (count_identicals == 4) {
                            cerr << "4 Identical coordinate points found in a single Node. Skip insertion" << endl;
                            return;
                        }
                    }
                    subdivide();
                    insert(_p, _offset);
                    return;
                }

                if (!root->isFull && root->isLeaf) {
                    root->add(_p, _offset);
                    return;
                }

                if (!root->isLeaf) {
                    long newX = (root->topLeft.x + root->botRight.x) / 2;
                    long newY = (root->topLeft.y + root->botRight.y) / 2;
                    Node<K> * tmp = root;
                    if (root->topLeft.x <= _p.x && _p.x <= newX)
                    {
                        if (root->topLeft.y >= _p.y && _p.y >= newY) {
                            if (root->nw == NULL) {
                                root->nw = new Node<K> (Point(root->topLeft.x, root->topLeft.y), Point(newX, newY));
                            }
                            root = root->nw;
                            insert(_p, _offset);
                        } else {
                            if (root->sw == NULL) {
                                root->sw = new Node<K> (Point(root->topLeft.x, newY), Point(newX, root->botRight.y));
                            }
                            root = root->sw;
                            insert(_p, _offset);
                        }
                    } else {
                        if (root->topLeft.y >= _p.y && _p.y >= newY) {
                            if (root->ne == NULL) {
                                root->ne = new Node<K> (Point(newX, root->topLeft.y), Point(root->botRight.x, newY));
                            }
                            root = root->ne;
                            insert(_p, _offset);
                        } else {
                            if (root->se == NULL) {
                                root->se = new Node<K> (Point(newX, newY), Point(root->botRight.x, root->botRight.y));
                            }
                            root = root->se;
                            insert(_p, _offset);
                        }
                    }
                    root = tmp;
                }
            }
        }

        vector<long> search (Point _p) {
            vector<long> result;

            if (!inBoundary(_p)) {
                // cerr << "Out of bound" << endl; // uncomment this line, to check if what_is_at and what_is_in actions working properly
                return {};
            }

            if (root == NULL) {
                return {};
            }

            if (root != NULL) {
                if (root->isLeaf) {
                    for (unsigned i = 0; i < root->coordinates.size(); i++) {
                        if (root->coordinates[i].x == _p.x && root->coordinates[i].y == _p.y)
                            result.push_back(root->record_offsets[i]);
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

