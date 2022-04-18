#ifndef SQL_PROJECT_PAGE_H
#define SQL_PROJECT_PAGE_H

#include <vector>


using namespace std;

struct Tuple{
    int x, y;
    Tuple(int x, int y){
        this->x = x;
        this->y = y;
    }
};

struct Page {
    Page(int size){
        this->size = size;
    }

    int size;
    bool insert(Tuple*);
    bool full = false;

    std::vector<Tuple*> tuples;
};

bool Page::insert(Tuple* tuple) {
    if (tuples.size() == size){
        return false;
    } else {
        tuples.push_back(tuple);

        if (tuples.size() == size){
            full = true;
        }

        return true;
    }
}

vector<Tuple*> mergeX(vector<Tuple*> left, vector<Tuple*> right){
    vector<Tuple*> merged;

    int a = 0;
    int b = 0;


    while(left.size() > a && right.size() > b){
        if (left[a]->x < right[b]->x){
            merged.push_back(left[a]);;
            a++;
        } else if (left[a]->x > right[b]->x){
            merged.push_back(right[b]);
            b++;
        } else if (left[a]->y < right[b]->y){
            merged.push_back(left[a]);;
            a++;
        } else {
            merged.push_back(right[b]);
            b++;
        }
    }

    if (left.size() > a){
        for(int i = a; i < left.size(); i++){
            merged.push_back(left[i]);
        }
    } else if (right.size() > b){
        for(int i = b; i < right.size(); i++){
            merged.push_back(right[i]);
        }
    }

    return merged;
}

vector<Tuple*> mergeSortX(vector<Tuple*> tuples){
    if (tuples.size() > 1){
        vector<Tuple*> left(tuples.begin(), tuples.begin() + tuples.size() / 2);
        vector<Tuple*> right(tuples.begin() + tuples.size() / 2, tuples.end());

        left = mergeSortX(left);
        right = mergeSortX(right);


        return mergeX(left, right);
    } else {
        return tuples;
    }
}

vector<Tuple*> mergeY(vector<Tuple*> left, vector<Tuple*> right){
    vector<Tuple*> merged;

    int a = 0;
    int b = 0;


    while(left.size() > a && right.size() > b){
        if (left[a]->y < right[b]->y){
            merged.push_back(left[a]);;
            a++;
        } else if (left[a]->y > right[b]->y){
            merged.push_back(right[b]);
            b++;
        } else if (left[a]->x < right[b]->x){
            merged.push_back(left[a]);;
            a++;
        } else {
            merged.push_back(right[b]);
            b++;
        }
    }

    if (left.size() > a){
        for(int i = a; i < left.size(); i++){
            merged.push_back(left[i]);
        }
    } else if (right.size() > b){
        for(int i = b; i < right.size(); i++){
            merged.push_back(right[i]);
        }
    }

    return merged;
}

vector<Tuple*> mergeSortY(vector<Tuple*> tuples){
    if (tuples.size() > 1){
        vector<Tuple*> left(tuples.begin(), tuples.begin() + tuples.size() / 2);
        vector<Tuple*> right(tuples.begin() + tuples.size() / 2, tuples.end());

        left = mergeSortY(left);
        right = mergeSortY(right);


        return mergeY(left, right);
    } else {
        return tuples;
    }
}

#endif
