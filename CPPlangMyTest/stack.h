#pragma once

#include <string>

// Объявление структуры узла стека.
struct StackNode {
    std::string data; // Данные, хранящиеся в узле.
    StackNode* next;  // Указатель на следующий узел в стеке.
};

// Объявление структуры стека.
class Stack {
public:
    StackNode* top; // Указатель на вершину стека.

    ~Stack() {
        destroy();
    }

    // Объявление методов для работы со стеком.
    void init(); // Инициализация стека.
    void push(const std::string& value); // Добавление элемента в стек (на вершину).
    void pop(); // Удаление элемента из стека (с вершины).
    void print(); // Вывод содержимого стека.
    void destroy(); // Освобождение памяти, занимаемой стеком.
    void loadFromFile(const std::string& fileName); // Загрузка стека из файла.
    void saveToFile(const std::string& fileName); // Сохранение стека в файл.
};

// Объявление функции для работы со стеком, вероятно, содержащей  основной цикл программы. Принимает аргументы командной строки.
void runStack(int argc, char* argv[]);