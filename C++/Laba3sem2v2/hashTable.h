#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>

struct HashNode {
    std::string key;
    std::string value;
    HashNode* next;
};

extern const int TABLE_SIZE;
extern HashNode* hashTable[];

void initTable();
int hashFunction(const std::string& key);  
void insert(const std::string& key, const std::string& value);
std::string get(const std::string& key);
void remove(const std::string& key);
void printTable();
void saveToFile(const std::string& fileName);
void loadFromFile(const std::string& fileName);
void freeTable();
void runHashTable(int argc, char* argv[]);

#endif




