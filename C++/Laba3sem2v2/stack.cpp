#include "stack.h"
#include "array.h"

#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

// Инициализация стека.
void Stack::init() {
    top = nullptr;
}

// Добавление элемента на вершину стека.
void Stack::push(const string& value) {
    StackNode* newNode = new StackNode{value, top};
    top = newNode;
}

// Удаление элемента с вершины стека.
void Stack::pop() {
    if (top == nullptr) {
        return;
    }
    StackNode* temp = top;
    top = top->next;
    delete temp;
}

// Вывод содержимого стека в консоль.
void Stack::print() {
    StackNode* temp = top;
    while (temp != nullptr) {
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << endl;
}

// Освобождение памяти, занимаемой стеком.
void Stack::destroy() {
    while (top != nullptr) {
        pop();
    }
}

// Загрузка стека из файла с использованием DynamicArray.
void Stack::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        return;
    }

    // Создаем временный динамический массив для хранения строк из файла.
    DynamicArray tempArray;

    string value;
    // Считываем все строки из файла в наш временный массив.
    while (file >> value) {
        tempArray.add(value);
    }
    file.close();

    // Теперь проходим по временному массиву С КОНЦА В НАЧАЛО,
    // чтобы правильно воссоздать стек.
    for (int i = tempArray.length() - 1; i >= 0; --i) {
        push(tempArray.get(i));
    }
}

// Сохранение стека в файл. Эта функция остается простой.
void Stack::saveToFile(const string& fileName) {
    ofstream file(fileName);
    StackNode* temp = top;
    // Просто проходим по стеку от вершины к основанию и записываем в файл.
    while (temp != nullptr) {
        file << temp->data << endl;
        temp = temp->next;
    }
    file.close();
}


// Функция для управления стеком через командную строку (остается без изменений).
void runStack(int argc, char* argv[]) {
    Stack stack;
    stack.init();

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

    if (!fileName.empty()) {
        stack.loadFromFile(fileName);
    }

    string command;
    size_t pos = query.find(' ');
    if (pos != string::npos) {
        command = query.substr(0, pos);
        query = query.substr(pos + 1);
    } else {
        command = query;
    }

    if (command == "SPUSH") {
        stack.push(query);
        if (!fileName.empty()) {
            stack.saveToFile(fileName);
        }
    } else if (command == "SPOP") {
        stack.pop();
        if (!fileName.empty()) {
            stack.saveToFile(fileName);
        }
    } else if (command == "SPRINT") {
        stack.print();
    }

    stack.destroy();
}