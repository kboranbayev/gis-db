#ifndef COORDINATEINDEX_H
#define COORDINATEINDEX_H

#include "GIS.h"

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

    void printPoint(void) {
        cout << "x = " << x << " y = " << y << " ";
    }
};

struct Node {
    Point pos;
    //string data;
    int data;
    Node (Point _pos, int _data) {
        pos = _pos;
        data = _data;
    }
    Node() {
        data = -1;
    }

    void printNode(void) {
        cout << "Point: ";
        pos.printPoint();
        cout << "\n Data: " << data << endl;
    }
    
};

class PRQuadTree {
    public:
        Node * node;
        
        Point topLeft;
        Point botRight;

        PRQuadTree *topLeftTree;
        PRQuadTree *topRightTree;
        PRQuadTree *botLeftTree;
        PRQuadTree *botRightTree;
        int data;
        int filled;
        bool isLeaf;
        
        PRQuadTree() {
            topLeft = Point(0, 0);
            botRight = Point(0, 0);
            node = NULL;
            topLeftTree  = NULL;
            topRightTree = NULL;
            botLeftTree  = NULL;
            botRightTree = NULL;
        }
        PRQuadTree(Point topL, Point botR) {
            node = NULL;
            topLeftTree  = NULL;
            topRightTree = NULL;
            botLeftTree  = NULL;
            botRightTree = NULL;
            topLeft = topL;
            botRight = botR;
            data = -1;
            filled = 0;
            isLeaf = true;
        }
        void insert(Node*);
        Node* search(Point);
        bool inBoundary(Point);
        void subdivide(void);
        void print(void) {
            if (node == NULL) {
                cout << "*";
            }
            if (node != NULL) {
                cout << "[(" << node->pos.y << "," << node->pos.x << "), " << data << "] ";
            }
            
            if (topLeftTree != NULL) {
                topLeftTree->print();
            } 
            if (topRightTree != NULL) {
                topRightTree->print();
            } 
            if (botLeftTree != NULL) {
                botLeftTree->print();
            } 
            if (botRightTree != NULL) {
                botRightTree->print();
            }
            cout << endl;
        }
};



void PRQuadTree::subdivide(void) {
    long X12 = (topLeft.x + botRight.x) / 2;
    long Y12 = (topLeft.y + botRight.y) / 2;

    if (topLeft.x <= node->pos.x && node->pos.x <= X12) {     // |X| |
        // Left Side
        if (topLeft.y >= node->pos.y && node->pos.y >= Y12) { //    |X| |
                                                              //    | | |
            // topLeft
            topLeftTree = new PRQuadTree(topLeft, Point(X12, Y12));
            topLeftTree->insert(node);
        } else {                                              //    | | |
                                                              //    |X| |
            // botLeft
            botLeftTree = new PRQuadTree(Point(topLeft.x, botRight.y), Point(X12, Y12));
            botLeftTree->insert(node);
        }
    } else {                                                   // | |X|
        // right side
        if (topLeft.y >= node->pos.y && node->pos.y >= Y12) { //    | |X|
                                                              //    | | |
            // topRight
            topRightTree = new PRQuadTree(Point(X12, Y12), Point(botRight.x, topLeft.y));
            topRightTree->insert(node);
        } else {                                              //    | | |
                                                              //    | |X|
            // botRight
            botRightTree = new PRQuadTree(Point(X12, Y12), botRight);
            botRightTree->insert(node);
        }
    }

}

void PRQuadTree::insert(Node * n) {
    if (n == NULL) {
        cout << "This should never be called\n";
        return;
    }
    
    // Current quad cannot contain it
    if (!inBoundary(n->pos)) {
        cout << "Not in boundary\n";
        return;
    }


   if (abs(topLeft.x - botRight.x) <= 1 &&
        abs(topLeft.y - botRight.y) <= 1)
    {
        if (node == NULL) {
            node = n;
            cout << "Node created" << endl;
            data = n->data;
            isLeaf = false;
            return;
        }
    }


    // if (!isLeaf && filled > 0) {
    //     cout << "Subdividing the region\n";
    //     subdivide();
    // }

    long newX = (topLeft.x + botRight.x) / 2;
    long newY = (topLeft.y + botRight.y) / 2;


    //cout << "(topLeft.x + botRight.x) / 2 = " << (topLeft.x + botRight.x) / 2 << " >= " << n->pos.x << endl;
    if (topLeft.x <= n->pos.x && n->pos.x <= newX)
    {
        // Indicates topLeftTree
        if (topLeft.y >= n->pos.y && n->pos.y >= newY)
        {
            if (topLeftTree == NULL)
                topLeftTree = new PRQuadTree(
                    Point(topLeft.x, topLeft.y),
                    Point(newX,
                        newY));
            // cout << "topLeftTree->insert()\n";
            topLeftTree->insert(n);
        }

        // Indicates botLeftTree
        else
        {
            if (botLeftTree == NULL)
                botLeftTree = new PRQuadTree(
                    Point(topLeft.x,
                        newY),
                    Point(newX,
                        botRight.y));
            //cout << "botLeftTree->insert()\n";
            botLeftTree->insert(n);
        }
    }
    else
    {
        // Indicates topRightTree
        // cout << "(topLeft.y + botRight.y) / 2 = " << (topLeft.y + botRight.y) / 2 << " <= " << n->pos.y << endl;
        if (topLeft.y >= n->pos.y && n->pos.y >= newY)
        {
            if (topRightTree == NULL)
                topRightTree = new PRQuadTree(
                    Point(newX,
                        topLeft.y),
                    Point(botRight.x,
                        newY));
            // cout << "topRightTree->insert()\n";
            topRightTree->insert(n);
        }

        // Indicates botRightTree
        else
        {
            // long newX = (topLeft.x + botRight.x) / 2;
            // long newY = (topLeft.y + botRight.y) / 2;

            if (botRightTree == NULL) {
                botRightTree = new PRQuadTree(
                    Point(newX, newY),
                    Point(botRight.x, botRight.y));
            }
            // cout << "botRightTree->insert()\n";
            // botRightTree->topLeft.printPoint();
            botRightTree->insert(n);
        }
    }
}

// Check if current quadtree contains the point
bool PRQuadTree::inBoundary(Point p)
{
    
    // cout << "p.y = " << p.y << " topLeft.y = " << topLeft.y << endl;
    // cout << "p.x >= topLeft.x == " << (p.x >= topLeft.x) << endl;
    // cout << "p.x <= botRight.x == " << (p.x <= botRight.x) << endl;
    // cout << "p.y <= topLeft.y == " << (p.y <= topLeft.y) << endl;
    // cout << "p.y >= botRight.y == " << (p.y >= botRight.y) << endl;
    /*
    if (!(p.x >= topLeft.x)) {
        cout << "p.x >= topLeft.x" << endl;
    }
    if (!(p.x <= botRight.x)) {
        cout << "p.x >= topLeft.x" << endl;
    }
    if (!(p.y <= topLeft.y)) {
        cout << "p.x >= topLeft.x" << endl;
    }
    if (!(p.y >= botRight.y)) {
        cout << "p.x >= topLeft.x" << endl;
    }
    */
    return (p.x >= topLeft.x &&
        p.x <= botRight.x &&
        p.y <= topLeft.y &&
        p.y >= botRight.y);
}

// Find a node in a quadtree
Node* PRQuadTree::search(Point p)
{
    // Current quad cannot contain it
    if (!inBoundary(p))
        return NULL;
  
    // We are at a quad of unit length
    // We cannot subdivide this quad further
    if (node != NULL)
        return node;
  
    if ((topLeft.x + botRight.x) / 2 >= p.x)
    {
        // Indicates topLeftTree
        if ((topLeft.y + botRight.y) / 2 >= p.y)
        {
            if (topLeftTree == NULL)
                return NULL;
            return topLeftTree->search(p);
        }
  
        // Indicates botLeftTree
        else
        {
            if (botLeftTree == NULL)
                return NULL;
            return botLeftTree->search(p);
        }
    }
    else
    {
        // Indicates topRightTree
        if ((topLeft.y + botRight.y) / 2 >= p.y)
        {
            if (topRightTree == NULL)
                return NULL;
            return topRightTree->search(p);
        }
  
        // Indicates botRightTree
        else
        {
            if (botRightTree == NULL)
                return NULL;
            return botRightTree->search(p);
        }
    }
}


#endif