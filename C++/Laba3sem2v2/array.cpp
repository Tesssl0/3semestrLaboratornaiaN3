#include "array.h"

#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

DynamicArray::DynamicArray(int initialCapacity) {
    // Безопаснее проверить, что capacity > 0
    capacity = (initialCapacity > 0) ? initialCapacity : 10;
    data = new string[capacity];
    size = 0;
}

DynamicArray::~DynamicArray() {
    delete[] data;
}

// Изменяет ёмкость массива, увеличивая или уменьшая её
void DynamicArray::resize(int newCapacity) {
    string* newData = new string[newCapacity];
    for (int i = 0; i < size; i++) {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

// Добавляет новый элемент в конец массива
void DynamicArray::add(const string& value) {
    if (size == capacity) {
        resize(capacity * 2);
    }
    data[size++] = value;
}

// Вставляет элемент в заданную позицию, сдвигая остальные элементы
void DynamicArray::insert(int index, const string& value) {
    if (index < 0 || index > size) {
        return;
    }
    if (size == capacity) {
        resize(capacity * 2);
    }
    for (int i = size; i > index; i--) {
        data[i] = data[i - 1];
    }
    data[index] = value;
    size++;
}

// Удаляет элемент из массива по индексу и сдвигает остальные элементы
void DynamicArray::remove(int index) {
    if (index < 0 || index >= size) {
        return;
    }
    for (int i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }
    size--;
}

// Возвращает элемент по индексу, или пустую строку, если индекс некорректен
string DynamicArray::get(int index) const {
    if (index < 0 || index >= size) {
        return "";
    }
    return data[index];
}

// Устанавливает значение элемента по индексу
void DynamicArray::set(int index, const string& value) {
    if (index < 0 || index >= size) {
        return;
    }
    data[index] = value;
}

// Возвращает текущий размер массива
int DynamicArray::length() const { return size; }

// Выводит все элементы массива на экран
void DynamicArray::print() const {
    for (int i = 0; i < size; i++) {
        cout << data[i] << " ";
    }
    cout << endl;
}

void DynamicArray::clear() {
    // 1. Освобождаем старую память
    delete[] data;

    // 2. Устанавливаем параметры по умолчанию (как в конструкторе)
    capacity = 10; // или какая-то другая начальная ёмкость
    size = 0;

    // 3. Выделяем новую, пустую область памяти
    data = new string[capacity];
}

// Загружает данные из файла в массив, добавляя строки построчно
void DynamicArray::loadFromFile(const string& fileName) {
    if (fileName.empty()) return; // ДОБАВЛЕНА ПРОВЕРКА
    ifstream file(fileName);
    string value;
    while (getline(file, value)) {
        add(value);
    }
    file.close();
}

// Сохраняет содержимое массива в файл, записывая каждую строку в отдельную строку файла
void DynamicArray::saveToFile(const string& fileName) {
    if (fileName.empty()) return; // ДОБАВЛЕНА ПРОВЕРКА
    ofstream file(fileName);
    for (int i = 0; i < size; i++) {
        file << data[i] << endl;
    }
    file.close();
}

// Выполняет команды над динамическим массивом на основе аргументов командной строки
void runDynamicArray(int argc, char* argv[]) {
    DynamicArray arr;

    string fileName;
    string query;

    for (int i = 1; i < argc; i++) {      // Обработка аргументов командной строки
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            fileName = argv[i + 1];       // Получение имени файла из аргумента
            i++;
        }
        else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[i + 1];          // Получение запроса из аргумента
            i++;
        }
    }

    if (!fileName.empty()) { // ДОБАВЛЕНА ПРОВЕРКА
        arr.loadFromFile(fileName);
    }

    // Извлечение команды и параметров из запроса
    string command;
    size_t pos = query.find(' ');
    if (pos != string::npos) {
        command = query.substr(0, pos);   // Команда до первого пробела
        query = query.substr(pos + 1);    // Остальная часть запроса
    }
    else {
        command = query;                  // Если в запросе только команда
    }

    if (command == "MPUSH") {
        arr.add(query);                   // Добавление элемента в конец
        if (!fileName.empty()) arr.saveToFile(fileName);
    }
    else if (command == "MINSERT") {
        size_t pos = query.find(' ');
        int index = stoi(query.substr(0, pos));
        string value = query.substr(pos + 1);
        arr.insert(index, value);         // Вставка элемента по индексу
        if (!fileName.empty()) arr.saveToFile(fileName);
    }
    else if (command == "MDEL") {
        int index = stoi(query);
        arr.remove(index);                // Удаление элемента по индексу
        if (!fileName.empty()) arr.saveToFile(fileName);
    }
    else if (command == "MSET") {
        size_t pos = query.find(' ');
        int index = stoi(query.substr(0, pos));
        string value = query.substr(pos + 1);
        arr.set(index, value);            // Установка нового значения по индексу
        if (!fileName.empty()) arr.saveToFile(fileName);
    }
    else if (command == "MLEN") {
        cout << arr.length() << endl;     // Вывод текущего размера массива
    }
    else if (command == "MPRINT") {
        arr.print();                      // Вывод всех элементов массива
    }
    else if (command == "MGET") {
        int index = stoi(query);
        cout << arr.get(index) << endl;   // Вывод элемента по индексу
    }
}