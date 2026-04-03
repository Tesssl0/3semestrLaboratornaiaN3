#pragma once

#include <string>

// Узел двусвязного списка.
struct DlistNode {
    std::string data;     // Данные, хранящиеся в узле.
    DlistNode* next;      // Указатель на следующий узел.
    DlistNode* prev;      // Указатель на предыдущий узел.
};

// Двусвязный список с указателями на начало и конец.
class DlinkedList {
public:
    DlistNode* head;       // Указатель на первый элемент списка.
    DlistNode* tail;       // Указатель на последний элемент списка.

    ~DlinkedList() {
        destroy();
    }

    // Методы для работы с двусвязным списком.
    void init();                          // Инициализирует пустой список.
    void addToHead(const std::string& value);  // Добавить элемент в начало.
    void addToTail(const std::string& value);  // Добавить элемент в конец.
    void removeFromHead();                // Удалить элемент с головы.
    void removeFromTail();                // Удалить элемент с хвоста.
    void removeByValue(const std::string& value); // Удалить по значению.
    bool search(const std::string& value); // Поиск по значению.
    void print();                         // Вывод элементов списка.
    void destroy();                       // Очистка списка.
    void loadFromFile(const std::string& fileName); // Загрузка из файла.
    void saveToFile(const std::string& fileName);   // Сохранение в файл.
    void addBefore(const std::string& target, const std::string& value); // вставить до target
    void addAfter(const std::string& target, const std::string& value);  // вставить после target
    void removeBefore(const std::string& target); // удалить элемент до target
    void removeAfter(const std::string& target);  // удалить элемент после target
};

// Запуск списка с использованием командной строки.
void runLLinkedList(int argc, char* argv[]);
