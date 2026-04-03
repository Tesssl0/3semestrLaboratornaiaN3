#include "hashTable.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const int TABLE_SIZE = 10;
HashNode* hashTable[TABLE_SIZE] = {};

// -------------------------
// RAII-гвард дл€ очистки
// -------------------------
class HashTableGuard {
public:
    HashTableGuard() {
        initTable();   // очищаем таблицу при старте
    }
    ~HashTableGuard() {
        freeTable();   // очищаем таблицу при завершении программы
    }
};

// глобальный объект Ч работает автоматически
static HashTableGuard __hash_guard;


// -------------------------
// –еализаци€ функций
// -------------------------

void initTable() {
    freeTable(); // очищаем старые данные
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = nullptr;
    }
}

int hashFunction(const string& key) {
    int hash = 0;
    for (char ch : key) hash += ch;
    return hash % TABLE_SIZE;
}

void insert(const string& key, const string& value) {
    int index = hashFunction(key);
    HashNode* current = hashTable[index];
    HashNode* prev = nullptr;

    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;
            return;
        }
        prev = current;
        current = current->next;
    }

    HashNode* newNode = new HashNode{ key, value, nullptr };

    if (prev == nullptr)
        hashTable[index] = newNode;
    else
        prev->next = newNode;
}

string get(const string& key) {
    int index = hashFunction(key);
    HashNode* current = hashTable[index];

    while (current != nullptr) {
        if (current->key == key) return current->value;
        current = current->next;
    }
    return "NOT_FOUND";
}

void remove(const string& key) {
    int index = hashFunction(key);
    HashNode* current = hashTable[index];
    HashNode* prev = nullptr;

    while (current != nullptr && current->key != key) {
        prev = current;
        current = current->next;
    }

    if (current == nullptr) return;

    if (prev == nullptr)
        hashTable[index] = current->next;
    else
        prev->next = current->next;

    delete current;
}

void printTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        cout << "Bucket " << i << ": ";
        HashNode* current = hashTable[i];
        while (current != nullptr) {
            cout << "[" << current->key << ": " << current->value << "] ";
            current = current->next;
        }
        cout << endl;
    }
}

void saveToFile(const string& fileName) {
    if (fileName.empty()) return;
    ofstream file(fileName);
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = hashTable[i];
        while (current != nullptr) {
            file << current->key << " " << current->value << endl;
            current = current->next;
        }
    }
}

void loadFromFile(const string& fileName) {
    if (fileName.empty()) return;
    freeTable();
    initTable();

    ifstream file(fileName);
    string key, value;
    while (file >> key >> value) insert(key, value);
}

void freeTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = hashTable[i];
        while (current != nullptr) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
        hashTable[i] = nullptr;
    }
}

void runHashTable(int argc, char* argv[]) {
    initTable();

    string fileName;
    string query;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            fileName = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[i + 1];
            i++;
        }
    }

    if (!fileName.empty()) {
        loadFromFile(fileName);
    }

    string command;
    size_t pos = query.find(' ');
    if (pos != string::npos) {
        command = query.substr(0, pos);
        query = query.substr(pos + 1);
    }
    else {
        command = query;
    }

    if (command == "HSET") {
        size_t pos = query.find(' ');
        string key = query.substr(0, pos);
        string value = query.substr(pos + 1);
        insert(key, value);
        if (!fileName.empty()) saveToFile(fileName);
    }
    else if (command == "HGET") {
        cout << get(query) << endl;
    }
    else if (command == "HDEL") {
        remove(query);
        if (!fileName.empty()) saveToFile(fileName);
    }
    else if (command == "HPRINT") {
        printTable();
    }
}
