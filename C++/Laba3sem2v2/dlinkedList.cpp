#include "dlinkedList.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// Инициализация списка.
void DlinkedList::init() {
    head = nullptr;
    tail = nullptr;
}

// Добавление нового элемента в начало.
void DlinkedList::addToHead(const string& value) {
    DlistNode* newNode = new DlistNode{ value, head, nullptr };
    if (head != nullptr) {
        head->prev = newNode;
    }
    head = newNode;
    if (tail == nullptr) {
        tail = head;
    }
}

// Добавление нового элемента в конец.
void DlinkedList::addToTail(const string& value) {
    DlistNode* newNode = new DlistNode{ value, nullptr, tail };
    if (tail != nullptr) {
        tail->next = newNode;
    }
    tail = newNode;
    if (head == nullptr) {
        head = tail;
    }
}

// Удаление элемента с головы.
void DlinkedList::removeFromHead() {
    if (head == nullptr) {
        return;
    }
    DlistNode* temp = head;
    head = head->next;
    if (head != nullptr) {
        head->prev = nullptr;
    }
    else {
        tail = nullptr;
    }
    delete temp;
}

// Удаление элемента с хвоста.
void DlinkedList::removeFromTail() {
    if (tail == nullptr) {
        return;
    }
    DlistNode* temp = tail;
    tail = tail->prev;
    if (tail != nullptr) {
        tail->next = nullptr;
    }
    else {
        head = nullptr;
    }
    delete temp;
}

// Удаление узла по значению.
void DlinkedList::removeByValue(const string& value) {
    DlistNode* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) {
            if (temp->prev != nullptr) {
                temp->prev->next = temp->next;
            }
            else {
                head = temp->next;
            }
            if (temp->next != nullptr) {
                temp->next->prev = temp->prev;
            }
            else {
                tail = temp->prev;
            }
            delete temp;
            return;
        }
        temp = temp->next;
    }
}

// Поиск элемента по значению.
bool DlinkedList::search(const string& value) {
    DlistNode* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

// Вывод всех элементов списка.
void DlinkedList::print() {
    DlistNode* temp = head;
    while (temp != nullptr) {
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << endl;
}

// Очистка списка.
void DlinkedList::destroy() {
    while (head != nullptr) {
        removeFromHead();
    }
}

// Загрузка элементов из файла.
void DlinkedList::loadFromFile(const string& fileName) {
    if (fileName.empty()) return; // ДОБАВЛЕНА ПРОВЕРКА
    ifstream file(fileName);
    string value;
    while (file >> value) {
        addToTail(value);
    }
    file.close();
}

// Сохранение элементов списка в файл.
void DlinkedList::saveToFile(const string& fileName) {
    if (fileName.empty()) return; // ДОБАВЛЕНА ПРОВЕРКА
    ofstream file(fileName);
    DlistNode* temp = head;
    while (temp != nullptr) {
        file << temp->data << endl;
        temp = temp->next;
    }
    file.close();
}

// --------------------- Новые методы ---------------------

void DlinkedList::addBefore(const string& target, const string& value) {
    if (head == nullptr) return;

    // если target в голове — просто добавить в голову
    if (head->data == target) {
        addToHead(value);
        return;
    }

    DlistNode* node = head->next;
    while (node != nullptr && node->data != target) {
        node = node->next;
    }
    if (node == nullptr) return; // target не найден

    // вставляем перед node
    DlistNode* newNode = new DlistNode{ value, node, node->prev };
    if (node->prev != nullptr) node->prev->next = newNode;
    node->prev = newNode;
}

void DlinkedList::addAfter(const string& target, const string& value) {
    DlistNode* node = head;
    while (node != nullptr && node->data != target) {
        node = node->next;
    }
    if (node == nullptr) return; // target не найден

    DlistNode* newNode = new DlistNode{ value, node->next, node };
    if (node->next != nullptr) {
        node->next->prev = newNode;
    }
    else {
        // вставка после хвоста -> обновляем tail
        tail = newNode;
    }
    node->next = newNode;
}

void DlinkedList::removeBefore(const string& target) {
    if (head == nullptr) return;

    // если target находится в голове — ничего удалить
    if (head->data == target) return;

    // если второй элемент — удалить голову
    if (head->next != nullptr && head->next->data == target) {
        removeFromHead();
        return;
    }

    DlistNode* node = head->next;
    while (node != nullptr && node->data != target) {
        node = node->next;
    }
    if (node == nullptr) return; // target не найден

    DlistNode* toRemove = node->prev;
    if (toRemove == nullptr) return; // нет элемента перед target (безопасность)

    // удалить toRemove
    DlistNode* before = toRemove->prev;
    node->prev = before;
    if (before != nullptr) {
        before->next = node;
    }
    else {
        // удалён был head
        head = node;
    }
    delete toRemove;
}

void DlinkedList::removeAfter(const string& target) {
    DlistNode* node = head;
    while (node != nullptr && node->data != target) {
        node = node->next;
    }
    if (node == nullptr) return; // target не найден
    if (node->next == nullptr) return; // нет элемента после target

    DlistNode* toRemove = node->next;
    DlistNode* after = toRemove->next;
    node->next = after;
    if (after != nullptr) {
        after->prev = node;
    }
    else {
        // удалён был tail
        tail = node;
    }
    delete toRemove;
}

// ---------------------------------------------------------

// Функция запуска работы со списком, обработка команд из аргументов командной строки.
void runLLinkedList(int argc, char* argv[]) {
    DlinkedList list;
    list.init();

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

    if (!fileName.empty())
        list.loadFromFile(fileName);

    string command;
    string rest;
    size_t pos = query.find(' ');
    if (pos != string::npos) {
        command = query.substr(0, pos);
        rest = query.substr(pos + 1);
    }
    else {
        command = query;
    }

    // разбиваем остальную часть запроса на токены
    istringstream iss(rest);
    string token1, token2;
    iss >> token1;
    iss >> token2; // может быть пустым

    if (command == "DPUSH") {
        if (!token1.empty()) {
            list.addToHead(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }
    else if (command == "DAPPEND") {
        if (!token1.empty()) {
            list.addToTail(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }
    else if (command == "DREMOVEHEAD") {
        list.removeFromHead();
        if (!fileName.empty()) list.saveToFile(fileName);
    }
    else if (command == "DREMOVETAIL") {
        list.removeFromTail();
        if (!fileName.empty()) list.saveToFile(fileName);
    }
    else if (command == "DREMOVE") {
        if (!token1.empty()) {
            list.removeByValue(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }
    else if (command == "DSEARCH") {
        if (!token1.empty())
            cout << (list.search(token1) ? "true" : "false") << endl;
    }
    else if (command == "DPRINT") {
        list.print();
    }
    // Новые команды:
    else if (command == "DADDTO") {
        // формат: DADDTO <target> <value>  -> вставить <value> до <target>
        if (!token1.empty() && !token2.empty()) {
            list.addBefore(token1, token2);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }
    else if (command == "DADDAFTER") {
        // формат: DADDAFTER <target> <value>  -> вставить <value> после <target>
        if (!token1.empty() && !token2.empty()) {
            list.addAfter(token1, token2);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }
    else if (command == "DREMOVETO") {
        // формат: DREMOVETO <target>  -> удалить элемент до <target>
        if (!token1.empty()) {
            list.removeBefore(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }
    else if (command == "DREMOVEAFTER") {
        // формат: DREMOVEAFTER <target> -> удалить элемент после <target>
        if (!token1.empty()) {
            list.removeAfter(token1);
            if (!fileName.empty()) list.saveToFile(fileName);
        }
    }

    list.destroy();
}