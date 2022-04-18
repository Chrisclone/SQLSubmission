#ifndef SQL_PROJECT_INDEX_H
#define SQL_PROJECT_INDEX_H
#include "Page.h"
#include "Query.h"
#include <vector>

using namespace std;

class ParentIndex{
public:
    virtual void insert(Tuple*) = 0;
    virtual void populate(ifstream& file) = 0;
    virtual vector<Tuple*> query(Query& q) = 0;
    virtual void print() = 0;
};

class SequentialSearch: public ParentIndex {
private:
    vector<Tuple*> tuples;
public:
    void insert(Tuple* p) override;
    void populate(ifstream& file) override;
    void print() override;
    vector<Tuple*> query(Query& q) override;
};

void SequentialSearch::print(){}

void SequentialSearch::insert(Tuple* p) {
    tuples.push_back(p);
}

void SequentialSearch::populate(ifstream& file){
    string line;

    while(!file.eof()){
        getline(file, line);

        if (line.empty()){
            break;
        }

        int x = stoi(line.substr(0, line.find(',')));
        int y = stoi(line.substr(line.find(',') + 1));

        insert(new Tuple(x, y));
    }

}

vector<Tuple*> SequentialSearch::query(Query& q) {
    auto validTuples = vector<Tuple*>();

    for (auto i: tuples){
        if (i->x >= q.xMin && i->x <= q.xMax &&
            i->y >= q.yMin && i->y <= q.yMax){
            validTuples.push_back(i);
        }
    }

    return validTuples;
}

struct Node{
    Node* left  = nullptr;
    Node* right = nullptr;

    Page* page;
    int depth;
    int divider = -1; //when a node is a leaf page node divider will be set to -1

    Node(int pageSize, int depth){
        page = new Page(pageSize);
        this->depth = depth;
    }

    Node* split(){
        left = new Node(page->size, depth + 1);
        right = new Node(page->size, depth + 1);

        if (depth % 2 == 1){ //split along x
            page->tuples = mergeSortX(page->tuples);

            if (page->size % 2 == 1){ // will cause an error if page size is 0 but that isn't a realistic page size
                divider = page->tuples[page->size / 2 + 1]->x;
            } else {
                divider = (page->tuples[page->size / 2]->x + page->tuples[page->size / 2 - 1]->x) / 2;
            }

            for(auto i: page->tuples){
                if (i->x < divider){
                    left->page->insert(i);
                } else {
                    right->page->insert(i);
                }
            }

        } else { //split along y
            page->tuples = mergeSortY(page->tuples);

            if (page->size % 2 == 1){ // will cause an error if page size is 0 but that isn't a realistic page size
                divider = page->tuples[page->size / 2 + 1]->y;
            } else {
                divider = (page->tuples[page->size / 2]->y + page->tuples[page->size / 2 - 1]->y) / 2;
            }

            for(auto i: page->tuples){
                if (i->y < divider){
                    left->page->insert(i);
                } else {
                    right->page->insert(i);
                }
            }
        }

        delete page;
        page = nullptr;

        return this;
    }
};

class KDSearch: public ParentIndex {
private:
    Node* head = nullptr;
    int pageSize;
public:
    void insert(Tuple* p) override;
    void recursiveInsert(Node* node, Tuple* p);
    void populate(ifstream& file) override;
    void print() override;
    vector<Tuple*> query(Query& q) override;

    KDSearch(int pageSize){
        this->pageSize = pageSize;
    }
};

void KDSearch::print() {
    vector<Node*> v;
    v.push_back(head);

    while(!v.empty()){
        vector<Node*> vReplace;
        for(auto i: v){
            if (i->divider == -1){
               cout << '[';
               for(int j = 0; j < 5; j++){
                   cout << "(" <<  i->page->tuples[j]->x << "," << i->page->tuples[j]->y << ")";
               }
               cout << "]  ";
            } else {
                cout << "Divider Node: " << i->divider;
            }

            if(i->right != nullptr){
                vReplace.push_back(i->right);
                vReplace.push_back(i->left);
            }
        }
        cout << endl;
        v = vReplace;
    }
}

void KDSearch::recursiveInsert(Node* node, Tuple *p) {
    if (node->divider == -1){
        if(!node->page->full){
            node->page->insert(p);
        } else {
            recursiveInsert(node->split(), p);
        }
    } else if (node->depth % 2 == 1){ //x value comparison
        if (p->x >= node->divider){
            recursiveInsert(node->right, p);
        } else {
            recursiveInsert(node->left, p);
        }
    } else if (node->depth % 2 == 0){ //y value comparison
        if (p->y >= node->divider){
            recursiveInsert(node->right, p);
        } else {
            recursiveInsert(node->left, p);
        }
    }
}

void KDSearch::insert(Tuple* p){
    if (head == nullptr){
        head = new Node(pageSize, 1);
        head->page->insert(p);
    } else {
        recursiveInsert(head, p);
    }
}

void KDSearch::populate(ifstream& file){
    string line;

    while(!file.eof()){
        getline(file, line);

        if (line.empty()){
            break;
        }

        int x = stoi(line.substr(0, line.find(',')));
        int y = stoi(line.substr(line.find(',') + 1));

        insert(new Tuple(x, y));
    }
}

vector<Tuple*> KDSearch::query(Query& q){
    vector<Node*> v;
    v.push_back(head);

    vector<Tuple*> valids;

    while(!v.empty()){
        vector<Node*> vReplace;

        for(auto i: v){
            if (i->divider == -1){
                for (auto j: i->page->tuples){
                    if (j->x >= q.xMin && j->x <= q.xMax &&
                        j->y >= q.yMin && j->y <= q.yMax){
                        valids.push_back(j);
                    }
                }
            } else if (i->depth % 2 == 1) { //x values
                if (i->divider >= q.xMin){
                   vReplace.push_back(i->left);
                }
                if (i->divider <= q.xMax){
                    vReplace.push_back(i->right);
                }
            } else if (i->depth % 2 == 0) { //y values
                if (i->divider >= q.yMin){
                    vReplace.push_back(i->left);
                }
                if (i->divider <= q.yMax){
                    vReplace.push_back(i->right);
                }
            }

        }
        v = vReplace;
    }
    return valids;
}

struct MYKDNode{
    MYKDNode* left  = nullptr;
    MYKDNode* right = nullptr;

    Page* page;
    bool xSplit;

    int depth;
    int divider = -1; //when a node is a leaf page node divider will be set to -1

    MYKDNode(int pageSize, int depth){
        page = new Page(pageSize);
        this->depth = depth;
    }

    MYKDNode* split(){
        left  = new MYKDNode(page->size, depth + 1);
        right = new MYKDNode(page->size, depth + 1);

        auto leftsorted  = mergeSortX(page->tuples);
        auto rightsorted = mergeSortY(page->tuples);

        if (leftsorted[leftsorted.size() - 1]->x - leftsorted[0]->x >
            rightsorted[rightsorted.size() - 1]->y - rightsorted[0]->y){ //split along x if x range is greater than y range
            xSplit = true;

            page->tuples = leftsorted;

            if (page->size % 2 == 1){ // will cause an error if page size is 0 but that isn't a realistic page size
                divider = page->tuples[page->size / 2 + 1]->x;
            } else {
                divider = (page->tuples[page->size / 2]->x + page->tuples[page->size / 2 - 1]->x) / 2;
            }

            for(auto i: page->tuples){
                if (i->x < divider){
                    left->page->insert(i);
                } else {
                    right->page->insert(i);
                }
            }

        } else { //split along y since y range is greater
            page->tuples = rightsorted;

            xSplit = false;

            if (page->size % 2 == 1){ // will cause an error if page size is 0 but that isn't a realistic page size
                divider = page->tuples[page->size / 2 + 1]->y;
            } else {
                divider = (page->tuples[page->size / 2]->y + page->tuples[page->size / 2 - 1]->y) / 2;
            }

            for(auto i: page->tuples){
                if (i->y < divider){
                    left->page->insert(i);
                } else {
                    right->page->insert(i);
                }
            }
        }

        delete page;
        page = nullptr;

        return this;
    }
};

class MYKDSearch: public ParentIndex {
private:
    MYKDNode* head = nullptr;
    int pageSize;
public:
    void insert(Tuple* p) override;
    void recursiveInsert(MYKDNode* node, Tuple* p);
    void populate(ifstream& file) override;
    void print() override;
    vector<Tuple*> query(Query& q) override;

    MYKDSearch(int pageSize){
        this->pageSize = pageSize;
    }
};

void MYKDSearch::insert(Tuple* p){
    if (head == nullptr){
        head = new MYKDNode(pageSize, 1);
        head->page->insert(p);
    } else {
        recursiveInsert(head, p);
    }
}

void MYKDSearch::recursiveInsert(MYKDNode* node, Tuple* p){
    if (node->divider == -1){
        if (!node->page->full){
            node->page->insert(p);
        } else {
            recursiveInsert(node->split(), p);
        }
    } else if (node->xSplit){ //x value comparison
        if (p->x >= node->divider){
            recursiveInsert(node->right, p);
        } else {
            recursiveInsert(node->left, p);
        }
    } else { //y value comparison
        if (p->y >= node->divider){
            recursiveInsert(node->right, p);
        } else {
            recursiveInsert(node->left, p);
        }
    }
}

void MYKDSearch::populate(ifstream& file){
    string line;


    while(!file.eof()){
        getline(file, line);

        if (line.empty()){
            break;
        }

        int x = stoi(line.substr(0, line.find(',')));
        int y = stoi(line.substr(line.find(',') + 1));

        insert(new Tuple(x, y));
    }
}

void MYKDSearch::print(){
    vector<MYKDNode*> v;
    v.push_back(head);

    while(!v.empty()){
        vector<MYKDNode*> vReplace;
        for(auto i: v){
            if (i->divider == -1){
                cout << '[';
                for(int j = 0; j < 5; j++){
                    cout << "(" <<  i->page->tuples[j]->x << "," << i->page->tuples[j]->y << ")";
                }
                cout << "]  ";
            } else {
                if(i->xSplit){
                    cout << "Divider Node X: " << i->divider;
                } else {
                    cout << "Divider Node Y: " << i->divider;
                }
            }

            if(i->right != nullptr){
                vReplace.push_back(i->right);
                vReplace.push_back(i->left);
            }
        }
        cout << endl;
        v = vReplace;
    }
}

vector<Tuple*> MYKDSearch::query(Query& q){
    vector<MYKDNode*> v;
    v.push_back(head);

    vector<Tuple*> valids;

    while(!v.empty()){
        vector<MYKDNode*> vReplace;

        for(auto i: v){
            if (i->divider == -1){
                for (auto j: i->page->tuples){
                    if (j->x >= q.xMin && j->x <= q.xMax &&
                        j->y >= q.yMin && j->y <= q.yMax){
                        valids.push_back(j);
                    }
                }
            } else if (i->xSplit) { //x values
                if (i->divider >= q.xMin){
                    vReplace.push_back(i->left);
                }
                if (i->divider <= q.xMax){
                    vReplace.push_back(i->right);
                }
            } else { //y values
                if (i->divider >= q.yMin){
                    vReplace.push_back(i->left);
                }
                if (i->divider <= q.yMax){
                    vReplace.push_back(i->right);
                }
            }

        }
        v = vReplace;
    }
    return valids;
}

#endif
