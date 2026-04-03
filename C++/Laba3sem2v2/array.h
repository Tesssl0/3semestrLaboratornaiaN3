#pragma once

#include <string>

// Структура, представляющая динамический массив строк
class DynamicArray {
public:
    std::string* data;    // Указатель на массив строк
    int size;             // Текущий размер массива (число элементов)
    int capacity;         // Ёмкость массива (максимальное число элементов, которое массив может хранить)

    DynamicArray(int initialCapacity = 10); // Конструктор
    ~DynamicArray(); // Деструктор
    
    void resize(int newCapacity);                 // Изменение ёмкости массива
    void add(const std::string& value);           // Добавление элемента в конец массива
    void insert(int index, const std::string& value); // Вставка элемента в заданную позицию
    void remove(int index);                       // Удаление элемента по индексу
    std::string get(int index) const;                   // Получение элемента по индексу
    void set(int index, const std::string& value); // Установка значения элемента по индексу
    int length() const;                                 // Возвращение текущего размера массива
    void print() const;                                 // Вывод всех элементов массива на экран
    void clear();
    void loadFromFile(const std::string& fileName); // Загрузка элементов массива из файла
    void saveToFile(const std::string& fileName);   // Сохранение элементов массива в файл
};

void runDynamicArray(int argc, char* argv[]);
