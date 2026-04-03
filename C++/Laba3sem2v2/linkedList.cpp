#include "linkedList.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

void LinkedList::init() {
    head = nullptr;
    tail = nullptr;
}

void LinkedList::addToHead(const string& value) {
    ListNode* newNode = new ListNode{value, head};
    head = newNode;
    if (tail == nullptr) {
        tail = head;
    }
}

void LinkedList::addToTail(const string& value) {
    ListNode* newNode = new ListNode{value, nullptr};
    if (tail != nullptr) {
        tail->next = newNode;
    }
    tail = newNode;
    if (head == nullptr) {
        head = tail;
    }
}

void LinkedList::removeFromHead() {
    if (head == nullptr) {
        return;
    }
    ListNode* temp = head;
    head = head->next;
    if (head == nullptr) {
        tail = nullptr;
    }
    delete temp;
}

void LinkedList::removeFromTail() {
    if (tail == nullptr) {
        return;
    }
    if (head == tail) {
        delete head;
        head = nullptr;
        tail = nullptr;
        return;
    }
    ListNode* temp = head;
    while (temp->next != tail) {
        temp = temp->next;
    }
    delete tail;
    tail = temp;
    tail->next = nullptr;
}

void LinkedList::removeByValue(const string& value) {
    if (head == nullptr) {
        return;
    }
    if (head->data == value) {
        removeFromHead();
        return;
    }
    ListNode* temp = head;
    while (temp->next != nullptr) {
        if (temp->next->data == value) {
            ListNode* nodeToRemove = temp->next;
            temp->next = temp->next->next;
            if (nodeToRemove == tail) {
                tail = temp;
            }
            delete nodeToRemove;
            return;
        }
        temp = temp->next;
    }
}

bool LinkedList::search(const string& value) {
    ListNode* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void LinkedList::print() {
    ListNode* temp = head;
    while (temp != nullptr) {
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << endl;
}

void LinkedList::destroy() {
    while (head != nullptr) {
        removeFromHead();
    }
}

void LinkedList::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    string value;
    while (file >> value) {
        addToTail(value);
    }
    file.close();
}

void LinkedList::saveToFile(const string& fileName) {
    ofstream file(fileName);
    ListNode* temp = head;
    while (temp != nullptr) {
        file << temp->data << endl;
        temp = temp->next;
    }
    file.close();
}

void LinkedList::addBefore(const string& target, const string& value) {
    if (head == nullptr) return;

    if (head->data == target) {
        addToHead(value);
        return;
    }

    ListNode* prev = head;
    while (prev->next != nullptr && prev->next->data != target) {
        prev = prev->next;
    }
    if (prev->next == nullptr) {
        return;
    }

    ListNode* newNode = new ListNode{value, prev->next};
    prev->next = newNode;
}

void LinkedList::addAfter(const string& target, const string& value) {
    ListNode* node = head;
    while (node != nullptr && node->data != target) {
        node = node->next;
    }
    if (node == nullptr) return;

    ListNode* newNode = new ListNode{value, node->next};
    node->next = newNode;
    if (node == tail) {
        tail = newNode;
    }
}

void LinkedList::removeBefore(const string& target) {
    if (head == nullptr) return;
    if (head->data == target) return;
    if (head->next != nullptr && head->next->data == target) {
        removeFromHead();
        return;
    }

    ListNode* prev = head;
    while (prev->next != nullptr && prev->next->next != nullptr && prev->next->next->data != target) {
        prev = prev->next;
    }
    if (prev->next == nullptr || prev->next->next == nullptr) {
        return;
    }
    // remove prev->next
    ListNode* nodeToRemove = prev->next;
    prev->next = nodeToRemove->next;
    if (nodeToRemove == tail) {
        tail = prev;
    }
    delete nodeToRemove;
}

void LinkedList::removeAfter(const string& target) {
    ListNode* node = head;
    while (node != nullptr && node->data != target) {
        node = node->next;
    }
    if (node == nullptr) return;
    if (node->next == nullptr) return;

    ListNode* nodeToRemove = node->next;
    node->next = nodeToRemove->next;
    if (nodeToRemove == tail) {
        tail = node;
    }
    delete nodeToRemove;
}

// Функция запуска работы со списком, обработка команд из аргументов командной строки
void runLinkedList(int argc, char* argv[]) {
    LinkedList list;
    list.init();

    string fileName;
    string query;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            fileName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[i + 1];
            i++;
        }
    }

    if (!fileName.empty())
        list.loadFromFile(fileName);

    string command;
    string rest;
    size_t pos = query.find(' ');
    if (pos != string::npos) {
        command = query.substr(0, pos);
        rest = query.substr(pos + 1);
    } else {
        command = query;
    }

    // удобное разбивание аргументов запроса на токены (target / value)
    istringstream iss(rest);
    string token1, token2;
    iss >> token1;
    iss >> token2; // может остаться пустым

    if (command == "LPUSH") {
        if (!token1.empty()) {
            list.addToHead(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    } 
    else if (command == "LAPPEND") {
        if (!token1.empty()) {
            list.addToTail(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    } 
    else if (command == "LREMOVEHEAD") {
        list.removeFromHead();
        if (!fileName.empty()) list.saveToFile(fileName);
    } 
    else if (command == "LREMOVETAIL") {
        list.removeFromTail();
        if (!fileName.empty()) list.saveToFile(fileName);
    } 
    else if (command == "LREMOVE") {
        if (!token1.empty()) {
            list.removeByValue(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    } 
    else if (command == "LSEARCH") {
        if (!token1.empty())
            cout << (list.search(token1) ? "true" : "false") << endl;
    } 
    else if (command == "LPRINT") {
        list.print();
    } 
    else if (command == "LADDTO") {
        if (!token1.empty() && !token2.empty()) {
            list.addBefore(token1, token2);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    } 
    else if (command == "LADDAFTER") {
        // формат: LADDAFTER <target> <value>  -> вставить <value> после <target>
        if (!token1.empty() && !token2.empty()) {
            list.addAfter(token1, token2);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    } 
    else if (command == "LREMOVETO") {
        // формат: LREMOVETO <target>  -> удалить элемент до <target>
        if (!token1.empty()) {
            list.removeBefore(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    } 
    else if (command == "LREMOVEAFTER") {
        // формат: LREMOVEAFTER <target> -> удалить элемент после <target>
        if (!token1.empty()) {
            list.removeAfter(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }

    list.destroy();
}