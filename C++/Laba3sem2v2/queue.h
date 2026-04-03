#pragma once

#include <string>

// Объявление структуры для узла очереди.
struct QueueNode {
    std::string data; // Данные, хранящиеся в узле.
    QueueNode* next;  // Указатель на следующий узел в очереди.
};

// Объявление структуры для самой очереди.
class Queue {
public:
    QueueNode* front; // Указатель на начало очереди (первый элемент).
    QueueNode* rear;  // Указатель на конец очереди (последний элемент).

    ~Queue() {
        destroy();
    }

    // Объявление методов для работы с очередью.
    void init(); // Инициализация очереди.
    void enqueue(const std::string& value); // Добавление элемента в конец очереди.
    void dequeue(); // Удаление элемента из начала очереди.
    void print(); // Вывод содержимого очереди.
    void destroy(); // Освобождение памяти, занимаемой очередью.
    void loadFromFile(const std::string& fileName); // Загрузка очереди из файла.
    void saveToFile(const std::string& fileName); // Сохранение очереди в файл.
};

// Объявление функции для работы с очередью, вероятно, содержащей основной цикл программы. Принимает аргументы командной строки.
void runQueue(int argc, char* argv[]);