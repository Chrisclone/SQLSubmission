#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "Query.h"
#include "Index.h"
using namespace std;

void processInput(int argc, char** argv, int& indexType, string& database, vector<Query>& queries, int& pageSize);


int main(int argc, char** argv) {
    int indexType, pageSize;
    string database;
    vector<Query> queries;

    processInput(argc, argv, indexType, database, queries, pageSize);

    ifstream file;
    file.open(database);

    ParentIndex* index = nullptr;

    switch (indexType){
        case 0:
            index = new SequentialSearch();
            break;
        case 1:
            index = new KDSearch(pageSize);
            break;
        case 2:
            index = new MYKDSearch(pageSize);
            break;
        default:
            cout << "Bad indexType value, chose a value in [0, 1, 2]" << endl;
            return -1;
    }


    index->populate(file);

    ofstream out;
    string testName = string(argv[3]);
    testName = testName.substr(0, testName.find('.'));

    out.open(database + "-out-" + to_string(indexType) + "-" + testName + ".txt");


    for(auto i: queries){
        auto valid = index->query(i);
        valid = mergeSortX(valid);

        for(auto j: valid){
            out << j->x << " " << j->y << endl;
        }

    }

    return 0;
}

void processInput(int argc, char** argv, int& indexType, string& database, vector<Query>& queries, int& pageSize){

    indexType = stoi(argv[1]);
    database = argv[2];

    ifstream file;
    file.open(argv[3]);

    string line;

    while(!file.eof()){
        getline(file, line);

        if (line.empty()){
            break;
        }

        vector<string> queryBuilder;

        string a = line.substr(0, line.find(' '));
        line = line.substr(line.find(' ') + 1);
        string b = line.substr(0, line.find(' '));
        line = line.substr(line.find(' ') + 1);
        string c = line.substr(0, line.find(' '));
        string d = line.substr(line.find(' ') + 1);

        queries.emplace_back(Query(a,b,c,d));
    }
    if (indexType != 0){
        pageSize = stoi(argv[argc - 1]);
    } else {
        pageSize = -1;
    }
}