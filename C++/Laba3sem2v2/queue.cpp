#include "queue.h"
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

void Queue::init() {
    front = nullptr; // Инициализация указателя на начало очереди.
    rear = nullptr;  // Инициализация указателя на конец очереди.
}

void Queue::enqueue(const string& value) {
    QueueNode* newNode = new QueueNode{value, nullptr}; // Создание нового узла.
    if (rear != nullptr) {
        rear->next = newNode; // Связывание нового узла с текущим последним узлом.
    }
    rear = newNode; // Обновление rear на новый узел.
    if (front == nullptr) {
        front = rear; // Установка front, если очередь была пустой.
    }
}

void Queue::dequeue() {
    if (front == nullptr) {
        return; // Проверка на пустую очередь.
    }
    QueueNode* temp = front; // Сохранение текущего первого узла для удаления.
    front = front->next; // Перемещение front на следующий узел.
    if (front == nullptr) {
        rear = nullptr; // Обновление rear, если очередь стала пустой.
    }
    delete temp; // Освобождение памяти удаляемого узла.
}

void Queue::print() {
    QueueNode* temp = front; // Начало с первого узла.
    while (temp != nullptr) {
        cout << temp->data << " "; // Вывод данных узла.
        temp = temp->next; // Переход к следующему узлу.
    }
    cout << endl; // Печать новой строки после вывода всех элементов.
}

void Queue::destroy() {
    while (front != nullptr) {
        dequeue(); // Освобождение памяти всех узлов.
    }
}

void Queue::loadFromFile(const string& fileName) {
    ifstream file(fileName); // Открытие файла для чтения.
    string value;
    while (file >> value) {
        enqueue(value); // Добавление значений из файла в очередь.
    }
    file.close(); // Закрытие файла.
}

void Queue::saveToFile(const string& fileName) {
    ofstream file(fileName); // Открытие файла для записи.
    QueueNode* temp = front; // Начало с первого узла.
    while (temp != nullptr) {
        file << temp->data << endl; // Запись данных узла в файл.
        temp = temp->next; // Переход к следующему узлу.
    }
    file.close(); // Закрытие файла.
}

void runQueue(int argc, char* argv[]) {
    Queue queue;
    queue.init(); // Инициализация очереди.

    string fileName;
    string query;

    // Обработка аргументов командной строки для получения имени файла и запроса.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            fileName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) {
            query = argv[i + 1];
            i++;
        }
    }

    queue.loadFromFile(fileName); // Загрузка данных из файла в очередь.

    string command;
    size_t pos = query.find(' '); // Поиск пробела для разделения команды и аргумента.
    if (pos != string::npos) {
        command = query.substr(0, pos); // Извлечение команды.
        query = query.substr(pos + 1); // Извлечение аргумента.
    } else {
        command = query; // Если пробела нет, вся строка - это команда.
    }

    // Выполнение команды в зависимости от введенного запроса.
    if (command == "QPUSH") {
        queue.enqueue(query); // Добавление элемента в очередь.
        queue.saveToFile(fileName); // Сохранение состояния очереди в файл.
    } else if (command == "QPOP") {
        queue.dequeue(); // Удаление элемента из очереди.
        queue.saveToFile(fileName); // Сохранение состояния очереди в файл.
    } else if (command == "QPRINT") {
        queue.print(); // Вывод элементов очереди.
    }

    queue.destroy(); // Освобождение памяти перед завершением программы.
}
