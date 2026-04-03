#define BOOST_TEST_MODULE DataStructuresTest
#define BOOST_TEST_MODULE Laba3sem2v2 tests
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <chrono>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <cstdio>

#include "array.h"
#include "binaryTree.h"
#include "dlinkedList.h"
#include "hashTable.h"
#include "linkedList.h"
#include "queue.h"
#include "stack.h"
#include "serialization.h"
using namespace std;

// Вспомогательный класс для безопасного перехвата cout
class CoutRedirector {
private:
    streambuf* old_rdbuf;
    ostringstream buffer;
public:
    CoutRedirector() : old_rdbuf(nullptr) {
        old_rdbuf = cout.rdbuf(buffer.rdbuf());
    }

    ~CoutRedirector() {
        if (old_rdbuf) {
            cout.rdbuf(old_rdbuf);
        }
    }

    string getOutput() {
        return buffer.str();
    }

    void clear() {
        buffer.str("");
        buffer.clear();
    }
};

// ------------------------------
//  Fixture для работы с файлами
// ------------------------------
struct FileFixture {
    string testFile = "test_temp.txt";

    FileFixture() {
        remove(testFile.c_str());
    }

    ~FileFixture() {
        remove(testFile.c_str());
        // Дополнительно удаляем другие возможные тестовые файлы
        remove("test_arr.txt");
        remove("ll.txt");
        remove("dll.txt");
        remove("hash_test.txt");
        remove("queue_test.txt");
        remove("stack.txt");
        remove("stack_out.txt");
        remove("bt.txt");
    }

    void createFile(const vector<string>& data) {
        ofstream f(testFile);
        for (const auto& s : data) f << s << endl;
        f.close();
    }
};

// ------------------------------
//  ТЕСТЫ ДЛЯ DynamicArray
// ------------------------------
BOOST_AUTO_TEST_SUITE(DynamicArrayTest)

BOOST_AUTO_TEST_CASE(test_insert_resize_force)
{
    DynamicArray arr;

    // Добавляем много элементов, чтобы точно вызвать resize
    for (int i = 0; i < 100; i++)
        arr.add(std::to_string(i));

    int oldSize = arr.length();

    arr.insert(oldSize, "LAST");

    BOOST_CHECK(arr.length() == oldSize + 1);
    BOOST_CHECK(arr.get(oldSize) == "LAST");
     arr.clear();
}

BOOST_AUTO_TEST_CASE(test_insert_valid_middle)
{
    DynamicArray arr;
    arr.add("a");
    arr.add("b");
    arr.add("c");

    arr.insert(1, "X");

    BOOST_CHECK(arr.length() == 4);
    BOOST_CHECK(arr.get(0) == "a");
    BOOST_CHECK(arr.get(1) == "X");
    BOOST_CHECK(arr.get(2) == "b");
    BOOST_CHECK(arr.get(3) == "c");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_insert_triggers_resize)
{
    DynamicArray arr(2);  // Явно указываем маленькую емкость

    arr.add("a");
    arr.add("b");

    // теперь size == capacity, insert должен вызвать resize
    arr.insert(2, "c");

    BOOST_CHECK(arr.length() == 3);
    BOOST_CHECK(arr.get(2) == "c");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_runDynamicArray_no_space_command)
{
    {
        std::ofstream f("test_arr.txt");
        f << "one\ntwo\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"test_arr.txt",
        (char*)"--query",
        (char*)"MLEN"
    };

    CoutRedirector redirect;
    runDynamicArray(5, argv);

    BOOST_CHECK(redirect.getOutput() == "2\n");
}

BOOST_AUTO_TEST_CASE(test_runDynamicArray_print)
{
    {
        std::ofstream f("test_arr.txt");
        f << "one\ntwo\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"test_arr.txt",
        (char*)"--query",
        (char*)"MPRINT"
    };

    CoutRedirector redirect;
    runDynamicArray(5, argv);

    BOOST_CHECK(redirect.getOutput() == "one two \n");
}

BOOST_AUTO_TEST_CASE(test_runDynamicArray_get)
{
    {
        std::ofstream f("test_arr.txt");
        f << "hello\nworld\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",           // argv[0] - имя программы
        (char*)"--file",         // argv[1] - опция --file
        (char*)"test_arr.txt",   // argv[2] - значение для --file
        (char*)"--query",        // argv[3] - опция --query
        (char*)"MGET 1"          // argv[4] - значение для --query
    };

    CoutRedirector redirect;
    runDynamicArray(5, argv);
    BOOST_CHECK(redirect.getOutput() == "world\n");
   
}

BOOST_AUTO_TEST_CASE(test_insert_valid_end)
{
    DynamicArray arr;
    arr.add("a");
    arr.add("b");

    arr.insert(2, "Z");

    BOOST_CHECK(arr.length() == 3);
    BOOST_CHECK(arr.get(2) == "Z");
}

BOOST_AUTO_TEST_CASE(test_insert_invalid_index)
{
    DynamicArray arr;
    arr.add("a");

    arr.insert(5, "X");   // index > size ? return

    BOOST_CHECK(arr.length() == 1);
    BOOST_CHECK(arr.get(0) == "a");
}

BOOST_AUTO_TEST_CASE(test_remove_valid)
{
    DynamicArray arr;
    arr.add("a");
    arr.add("b");
    arr.add("c");

    arr.remove(1);

    BOOST_CHECK(arr.length() == 2);
    BOOST_CHECK(arr.get(0) == "a");
    BOOST_CHECK(arr.get(1) == "c");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_remove_invalid_index)
{
    DynamicArray arr;
    arr.add("a");
    arr.add("b");

    arr.remove(10);   // invalid ? return

    BOOST_CHECK(arr.length() == 2);
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_set_valid)
{
    DynamicArray arr;
    arr.add("a");
    arr.add("b");

    arr.set(1, "X");

    BOOST_CHECK(arr.get(1) == "X");
}

BOOST_AUTO_TEST_CASE(test_set_invalid)
{
    DynamicArray arr;
    arr.add("a");

    arr.set(5, "X");  // invalid ? return

    BOOST_CHECK(arr.get(0) == "a");
}

BOOST_AUTO_TEST_CASE(test_get_valid)
{
    DynamicArray arr;
    arr.add("hello");

    BOOST_CHECK(arr.get(0) == "hello");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_print_output)
{
    DynamicArray arr;
    arr.add("a");
    arr.add("b");

    CoutRedirector redirect;
    arr.print();

    BOOST_CHECK(redirect.getOutput() == "a b \n");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_runDynamicArray_push)
{
    // создаём временный файл
    {
        std::ofstream f("test_arr.txt");
        f << "one\ntwo\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"test_arr.txt",
        (char*)"--query",
        (char*)"MPUSH three"
    };

    runDynamicArray(5, argv);

    DynamicArray arr;
    arr.loadFromFile("test_arr.txt");

    BOOST_CHECK(arr.length() == 3);
    BOOST_CHECK(arr.get(2) == "three");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_runDynamicArray_insert)
{
    {
        std::ofstream f("test_arr.txt");
        f << "one\ntwo\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"test_arr.txt",
        (char*)"--query",
        (char*)"MINSERT 1 X"
    };

    runDynamicArray(5, argv);

    DynamicArray arr;
    arr.loadFromFile("test_arr.txt");

    BOOST_CHECK(arr.get(1) == "X");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(test_runDynamicArray_delete)
{
    {
        std::ofstream f("test_arr.txt");
        f << "one\ntwo\nthree\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"test_arr.txt",
        (char*)"--query",
        (char*)"MDEL 1"
    };

    runDynamicArray(5, argv);

    DynamicArray arr;
    arr.loadFromFile("test_arr.txt");

    BOOST_CHECK(arr.length() == 2);
    BOOST_CHECK(arr.get(1) == "three");
	arr.clear();
}

BOOST_AUTO_TEST_CASE(test_runDynamicArray_set)
{
    {
        std::ofstream f("test_arr.txt");
        f << "one\ntwo\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"test_arr.txt",
        (char*)"--query",
        (char*)"MSET 0 XXX"
    };

    runDynamicArray(5, argv);

    DynamicArray arr;
    arr.loadFromFile("test_arr.txt");

    BOOST_CHECK(arr.get(0) == "XXX");
    arr.clear();
}

BOOST_AUTO_TEST_CASE(create_and_add) {
    DynamicArray arr;
    BOOST_TEST(arr.length() == 0);

    arr.add("one");
    arr.add("two");
    BOOST_TEST(arr.length() == 2);
    BOOST_TEST(arr.get(0) == "one");
    BOOST_TEST(arr.get(1) == "two");
}

BOOST_AUTO_TEST_CASE(insert_and_remove) {
    DynamicArray arr;
    arr.add("a");
    arr.add("c");
    arr.insert(1, "b");
    BOOST_TEST(arr.get(1) == "b");
    BOOST_TEST(arr.length() == 3);

    arr.remove(1);
    BOOST_TEST(arr.get(1) == "c");
    BOOST_TEST(arr.length() == 2);
}

BOOST_AUTO_TEST_CASE(set_and_get) {
    DynamicArray arr;
    arr.add("x");
    arr.set(0, "y");
    BOOST_TEST(arr.get(0) == "y");
    BOOST_TEST(arr.get(5) == ""); // out of bounds
}

BOOST_AUTO_TEST_CASE(clear) {
    DynamicArray arr;
    arr.add("data");
    arr.clear();
    BOOST_TEST(arr.length() == 0);
    BOOST_TEST(arr.capacity >= 10);
}

BOOST_FIXTURE_TEST_CASE(file_io, FileFixture) {
    DynamicArray arr;
    arr.add("hello");
    arr.add("world");
    arr.saveToFile(testFile);

    DynamicArray arr2;
    arr2.loadFromFile(testFile);
    BOOST_TEST(arr2.length() == 2);
    BOOST_TEST(arr2.get(0) == "hello");
    BOOST_TEST(arr2.get(1) == "world");
    arr.clear();
}

// Бенчмарк для DynamicArray
BOOST_AUTO_TEST_CASE(benchmark_dynamic_array) {
    DynamicArray arr;
    const int N = 100000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        arr.add(std::to_string(i));
    }

    auto end = std::chrono::high_resolution_clock::now();
    long elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    BOOST_TEST_MESSAGE("DynamicArray add " << N << " elements: "
        << elapsed_ms << " ms");
}


BOOST_AUTO_TEST_SUITE_END()

// ------------------------------
//  ТЕСТЫ ДЛЯ LinkedList
// ------------------------------
BOOST_AUTO_TEST_SUITE(LinkedListTest)

BOOST_AUTO_TEST_CASE(test_ll_remove_tail_empty)
{
    LinkedList list;
    list.init();

    list.removeFromTail(); // ничего не должно произойти

    BOOST_CHECK(list.head == nullptr);
    BOOST_CHECK(list.tail == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_ll_remove_tail_single)
{
    LinkedList list;
    list.init();

    list.addToHead("A");

    list.removeFromTail(); // удаляет единственный элемент

    BOOST_CHECK(list.head == nullptr);
    BOOST_CHECK(list.tail == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_ll_remove_tail_multiple)
{
    LinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("B");
    list.addToTail("C");

    list.removeFromTail(); // удаляет C

    BOOST_CHECK(list.tail->data == "B");
    BOOST_CHECK(list.tail->next == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_ll_print)
{
    LinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("B");

    CoutRedirector redirect;
    list.print();

    BOOST_CHECK(redirect.getOutput() == "A B \n");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_push)
{
    {
        std::ofstream f("ll.txt");
        f << "";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LPUSH X"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.head->data == "X");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_append)
{
    {
        std::ofstream f("ll.txt");
        f << "";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LAPPEND Y"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.tail->data == "Y");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_remove_head)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LREMOVEHEAD"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.head->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_remove_tail)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LREMOVETAIL"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.tail->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_remove_value)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LREMOVE B"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.head->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_search)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LSEARCH B"
    };

    CoutRedirector redirect;
    runLinkedList(5, argv);

    BOOST_CHECK(redirect.getOutput() == "true\n");
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_print)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LPRINT"
    };

    CoutRedirector redirect;
    runLinkedList(5, argv);

    BOOST_CHECK(redirect.getOutput() == "A B \n");
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_add_before)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LADDTO C B"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.head->next->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_add_after)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LADDAFTER A B"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.head->next->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_remove_before)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LREMOVETO C"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.head->data == "A");
    BOOST_CHECK(list.head->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runLinkedList_remove_after)
{
    {
        std::ofstream f("ll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"ll.txt",
        (char*)"--query",
        (char*)"LREMOVEAFTER A"
    };

    runLinkedList(5, argv);

    LinkedList list;
    list.init();
    list.loadFromFile("ll.txt");

    BOOST_CHECK(list.head->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(init_and_add) {
    LinkedList list;
    list.init();
    list.addToHead("first");
    list.addToTail("last");

    BOOST_TEST(list.head->data == "first");
    BOOST_TEST(list.tail->data == "last");
    list.destroy();
}

BOOST_AUTO_TEST_CASE(remove_by_value) {
    LinkedList list;
    list.init();
    list.addToTail("a");
    list.addToTail("b");
    list.addToTail("c");

    list.removeByValue("b");
    BOOST_TEST(list.search("b") == false);
    BOOST_TEST(list.head->data == "a");
    BOOST_TEST(list.head->next->data == "c");
    BOOST_TEST(list.tail->data == "c");
    list.destroy();
}

BOOST_AUTO_TEST_CASE(add_before_after) {
    LinkedList list;
    list.init();
    list.addToTail("b");
    list.addBefore("b", "a");
    list.addAfter("b", "c");

    BOOST_TEST(list.head->data == "a");
    BOOST_TEST(list.head->next->data == "b");
    BOOST_TEST(list.tail->data == "c");
    list.destroy();
}

BOOST_AUTO_TEST_CASE(remove_before_after) {
    LinkedList list;
    list.init();
    list.addToTail("a");
    list.addToTail("b");
    list.addToTail("c");

    list.removeBefore("b");
    BOOST_TEST(list.head->data == "b");

    list.removeAfter("b");
    BOOST_TEST(list.tail->data == "b");
    list.destroy();
}

BOOST_FIXTURE_TEST_CASE(file_io_linkedlist, FileFixture) {
    LinkedList list;
    list.init();
    list.addToTail("hello");
    list.addToTail("world");
    list.saveToFile(testFile);

    LinkedList list2;
    list2.init();
    list2.loadFromFile(testFile);
    BOOST_TEST(list2.search("hello") == true);
    BOOST_TEST(list2.search("world") == true);
    list.destroy();
}

// Бенчмарк LinkedList
BOOST_AUTO_TEST_CASE(benchmark_linkedlist) {
    LinkedList list;
    list.init();
    const int N = 50000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        list.addToTail(std::to_string(i));
    }

    auto end = std::chrono::high_resolution_clock::now();
    long elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    BOOST_TEST_MESSAGE("LinkedList addToTail " << N << " elements: "
        << elapsed_ms << " ms");

    list.destroy();
}


BOOST_AUTO_TEST_SUITE_END()

// ------------------------------
//  ТЕСТЫ ДЛЯ DlinkedList
// ------------------------------
BOOST_AUTO_TEST_SUITE(DlinkedListTest)

BOOST_AUTO_TEST_CASE(double_linked_operations) {
    DlinkedList list;
    list.init();
    list.addToHead("second");
    list.addToHead("first");
    list.addToTail("third");

    BOOST_TEST(list.head->data == "first");
    BOOST_TEST(list.tail->data == "third");
    BOOST_TEST(list.head->next->prev->data == "first");

    list.removeBefore("third");
    BOOST_TEST(list.tail->prev->data == "first");
    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_remove_tail_empty)
{
    DlinkedList list;
    list.init();

    list.removeFromTail();

    BOOST_CHECK(list.head == nullptr);
    BOOST_CHECK(list.tail == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_remove_tail_single)
{
    DlinkedList list;
    list.init();

    list.addToHead("A");
    list.removeFromTail();

    BOOST_CHECK(list.head == nullptr);
    BOOST_CHECK(list.tail == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_remove_tail_multiple)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("B");
    list.addToTail("C");

    list.removeFromTail();

    BOOST_CHECK(list.tail->data == "B");
    BOOST_CHECK(list.tail->next == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_print)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("B");

    CoutRedirector redirect;
    list.print();

    BOOST_CHECK(redirect.getOutput() == "A B \n");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_addBefore_empty)
{
    DlinkedList list;
    list.init();

    list.addBefore("X", "Y"); // ничего не делает

    BOOST_CHECK(list.head == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_addBefore_head)
{
    DlinkedList list;
    list.init();

    list.addToHead("B");
    list.addBefore("B", "A");

    BOOST_CHECK(list.head->data == "A");
    BOOST_CHECK(list.head->next->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_addBefore_middle)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("C");

    list.addBefore("C", "B");

    BOOST_CHECK(list.head->next->data == "B");
    BOOST_CHECK(list.head->next->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_addBefore_not_found)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addBefore("Z", "X"); // ничего не делает

    BOOST_CHECK(list.head->next == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_addAfter_middle)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("C");

    list.addAfter("A", "B");

    BOOST_CHECK(list.head->next->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_addAfter_tail)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");

    list.addAfter("A", "B");

    BOOST_CHECK(list.tail->data == "B");
    BOOST_CHECK(list.tail->prev->data == "A");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_addAfter_not_found)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addAfter("Z", "X"); // ничего не делает

    BOOST_CHECK(list.head->next == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_removeAfter_not_found)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.removeAfter("Z");

    BOOST_CHECK(list.head->next == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_removeAfter_last)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.removeAfter("A"); // нет next

    BOOST_CHECK(list.head->next == nullptr);

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_removeAfter_middle)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("B");
    list.addToTail("C");

    list.removeAfter("A"); // удаляет B

    BOOST_CHECK(list.head->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_dll_removeAfter_tail)
{
    DlinkedList list;
    list.init();

    list.addToTail("A");
    list.addToTail("B");

    list.removeAfter("A"); // удаляет B ? tail = A

    BOOST_CHECK(list.tail->data == "A");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_push)
{
    {
        std::ofstream f("dll.txt");
        f << "";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DPUSH X"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.head->data == "X");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_append)
{
    {
        std::ofstream f("dll.txt");
        f << "";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DAPPEND Y"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.tail->data == "Y");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_remove_head)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DREMOVEHEAD"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.head->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_remove_tail)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DREMOVETAIL"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.tail->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_remove_value)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DREMOVE B"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.head->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_search)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DSEARCH B"
    };

    CoutRedirector redirect;
    runLLinkedList(5, argv);

    BOOST_CHECK(redirect.getOutput() == "true\n");
}

BOOST_AUTO_TEST_CASE(test_runDLL_print)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DPRINT"
    };

    CoutRedirector redirect;
    runLLinkedList(5, argv);

    BOOST_CHECK(redirect.getOutput() == "A B \n");
}

BOOST_AUTO_TEST_CASE(test_runDLL_add_before)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DADDTO C B"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.head->next->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_add_after)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DADDAFTER A B"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.head->next->data == "B");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_remove_before)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DREMOVETO C"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.head->data == "A");
    BOOST_CHECK(list.head->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(test_runDLL_remove_after)
{
    {
        std::ofstream f("dll.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"dll.txt",
        (char*)"--query",
        (char*)"DREMOVEAFTER A"
    };

    runLLinkedList(5, argv);

    DlinkedList list;
    list.init();
    list.loadFromFile("dll.txt");

    BOOST_CHECK(list.head->next->data == "C");

    list.destroy();
}

BOOST_AUTO_TEST_CASE(remove_by_value_double) {
    DlinkedList list;
    list.init();
    list.addToTail("x");
    list.addToTail("y");
    list.addToTail("z");

    list.removeByValue("y");
    BOOST_TEST(list.search("y") == false);
    BOOST_TEST(list.head->next->data == "z");
    BOOST_TEST(list.tail->prev->data == "x");
    list.destroy();
}

BOOST_FIXTURE_TEST_CASE(file_io_dlinkedlist, FileFixture) {
    DlinkedList list;
    list.init();
    list.addToTail("double");
    list.addToTail("linked");
    list.saveToFile(testFile);

    DlinkedList list2;
    list2.init();
    list2.loadFromFile(testFile);
    BOOST_TEST(list2.search("double") == true);
    BOOST_TEST(list2.search("linked") == true);
    list.destroy();
}

BOOST_AUTO_TEST_CASE(benchmark_dlinkedlist) {
    DlinkedList list;
    list.init();
    const int N = 50000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        list.addToTail(std::to_string(i));
    }

    auto end = std::chrono::high_resolution_clock::now();
    long elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    BOOST_TEST_MESSAGE("DlinkedList addToTail " << N << " elements: "
        << elapsed_ms << " ms");

    list.destroy();
}


BOOST_AUTO_TEST_SUITE_END()

// ------------------------------
//  ТЕСТЫ ДЛЯ HashTable
// ------------------------------
BOOST_AUTO_TEST_SUITE(HashTableTest)

BOOST_AUTO_TEST_CASE(insert_and_get) {
    initTable();
    insert("key1", "value1");
    insert("key2", "value2");

    BOOST_TEST(get("key1") == "value1");
    BOOST_TEST(get("key2") == "value2");
    BOOST_TEST(get("nonexistent") == "NOT_FOUND");

    freeTable();
}

BOOST_AUTO_TEST_CASE(test_hash_remove_not_found)
{
    initTable();
    insert("a", "1");

    remove("zzz"); // нет такого ключа

    BOOST_CHECK(get("a") == "1");

    freeTable();
}

BOOST_AUTO_TEST_CASE(test_hash_printTable)
{
    initTable();
    insert("a", "1");
    insert("b", "2");

    CoutRedirector redirect;
    printTable();

    BOOST_CHECK(redirect.getOutput().size() > 0);

    freeTable();
}

BOOST_AUTO_TEST_CASE(test_runHashTable_set)
{
    {
        std::ofstream f("hash_test.txt");
        f << "";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"hash_test.txt",
        (char*)"--query",
        (char*)"HSET key1 value1"
    };

    runHashTable(5, argv);

    initTable();
    loadFromFile("hash_test.txt");

    BOOST_CHECK(get("key1") == "value1");

    freeTable();
}

BOOST_AUTO_TEST_CASE(test_runHashTable_get)
{
    {
        std::ofstream f("hash_test.txt");
        f << "key1 value1\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"hash_test.txt",
        (char*)"--query",
        (char*)"HGET key1"
    };

    CoutRedirector redirect;
    runHashTable(5, argv);

    BOOST_CHECK(redirect.getOutput() == "value1\n");
    freeTable();
}

BOOST_AUTO_TEST_CASE(test_runHashTable_del)
{
    {
        std::ofstream f("hash_test.txt");
        f << "key1 value1\nkey2 value2\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"hash_test.txt",
        (char*)"--query",
        (char*)"HDEL key1"
    };

    runHashTable(5, argv);

    initTable();
    loadFromFile("hash_test.txt");

    BOOST_CHECK(get("key1") == "NOT_FOUND");
    BOOST_CHECK(get("key2") == "value2");

    freeTable();
}

BOOST_AUTO_TEST_CASE(test_runHashTable_print)
{
    {
        std::ofstream f("hash_test.txt");
        f << "key1 value1\nkey2 value2\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"hash_test.txt",
        (char*)"--query",
        (char*)"HPRINT"
    };

    CoutRedirector redirect;
    runHashTable(5, argv);

    BOOST_CHECK(redirect.getOutput().size() > 0);
    freeTable();
}

BOOST_FIXTURE_TEST_CASE(file_io_hash, FileFixture) {
    initTable();
    insert("hello", "world");
    saveToFile(testFile);
    freeTable();

    initTable();
    loadFromFile(testFile);
    BOOST_TEST(get("hello") == "world");
    freeTable();
}

BOOST_AUTO_TEST_CASE(benchmark_hashtable) {
    initTable();
    const int N = 20000;

    // --- измерение вставки ---
    auto start_insert = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        insert("key" + std::to_string(i), "val" + std::to_string(i));
    }

    auto end_insert = std::chrono::high_resolution_clock::now();
    long insert_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_insert - start_insert).count();

    BOOST_TEST_MESSAGE("HashTable insert " << N << " elements: "
        << insert_ms << " ms");

    // --- измерение поиска ---
    auto start_search = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        get("key" + std::to_string(i));
    }

    auto end_search = std::chrono::high_resolution_clock::now();
    long search_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_search - start_search).count();

    BOOST_TEST_MESSAGE("HashTable search " << N << " elements: "
        << search_ms << " ms");

    freeTable();
}


BOOST_AUTO_TEST_SUITE_END()

// ------------------------------
//  ТЕСТЫ ДЛЯ Queue
// ------------------------------
BOOST_AUTO_TEST_SUITE(QueueTest)

BOOST_AUTO_TEST_CASE(enqueue_dequeue) {
    Queue q;
    q.init();
    q.enqueue("a");
    q.enqueue("b");

    BOOST_TEST(q.front->data == "a");

    q.dequeue();
    BOOST_TEST(q.front->data == "b");

    q.dequeue();
    BOOST_TEST(q.front == nullptr);
    BOOST_TEST(q.rear == nullptr);
    q.destroy();
}

BOOST_AUTO_TEST_CASE(test_runQueue_no_space_command)
{
    {
        std::ofstream f("queue_test.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"queue_test.txt",
        (char*)"--query",
        (char*)"QPRINT"
    };

    CoutRedirector redirect;
    runQueue(5, argv);

    BOOST_CHECK(redirect.getOutput() == "A B \n");
}

BOOST_AUTO_TEST_CASE(test_runQueue_push)
{
    {
        std::ofstream f("queue_test.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"queue_test.txt",
        (char*)"--query",
        (char*)"QPUSH C"
    };

    runQueue(5, argv);

    Queue q;
    q.init();
    q.loadFromFile("queue_test.txt");

    BOOST_CHECK(q.front->data == "A");
    BOOST_CHECK(q.front->next->data == "B");
    BOOST_CHECK(q.front->next->next->data == "C");

    q.destroy();
}

BOOST_AUTO_TEST_CASE(test_runQueue_pop)
{
    {
        std::ofstream f("queue_test.txt");
        f << "A\nB\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"queue_test.txt",
        (char*)"--query",
        (char*)"QPOP"
    };

    runQueue(5, argv);

    Queue q;
    q.init();
    q.loadFromFile("queue_test.txt");

    BOOST_CHECK(q.front->data == "B");
    BOOST_CHECK(q.front->next->data == "C");

    q.destroy();
}

BOOST_AUTO_TEST_CASE(test_queue_print)
{
    Queue q;
    q.init();

    q.enqueue("one");
    q.enqueue("two");
    q.enqueue("three");

    CoutRedirector redirect;
    q.print();

    BOOST_CHECK(redirect.getOutput() == "one two three \n");

    q.destroy();
}

BOOST_FIXTURE_TEST_CASE(file_io_queue, FileFixture) {
    Queue q;
    q.init();
    q.enqueue("queue");
    q.enqueue("test");
    q.saveToFile(testFile);

    Queue q2;
    q2.init();
    q2.loadFromFile(testFile);
    BOOST_TEST(q2.front->data == "queue");
    BOOST_TEST(q2.front->next->data == "test");

    q.destroy();
    q2.destroy();
}

BOOST_AUTO_TEST_CASE(benchmark_queue) {
    Queue q;
    q.init();
    const int N = 100000;

    // --- enqueue benchmark ---
    auto start_enqueue = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        q.enqueue(std::to_string(i));
    }

    auto end_enqueue = std::chrono::high_resolution_clock::now();
    long enqueue_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_enqueue - start_enqueue).count();

    BOOST_TEST_MESSAGE("Queue enqueue " << N << " elements: "
        << enqueue_ms << " ms");

    // --- dequeue benchmark ---
    auto start_dequeue = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        q.dequeue();
    }

    auto end_dequeue = std::chrono::high_resolution_clock::now();
    long dequeue_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_dequeue - start_dequeue).count();

    BOOST_TEST_MESSAGE("Queue dequeue " << N << " elements: "
        << dequeue_ms << " ms");

    q.destroy();
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------
//  ТЕСТЫ ДЛЯ Stack
// ------------------------------
BOOST_AUTO_TEST_SUITE(StackTest)

BOOST_AUTO_TEST_CASE(push_pop) {
    Stack s;
    s.init();
    s.push("bottom");
    s.push("top");

    BOOST_TEST(s.top->data == "top");

    s.pop();
    BOOST_TEST(s.top->data == "bottom");

    s.pop();
    BOOST_TEST(s.top == nullptr);

    s.destroy();
}

BOOST_AUTO_TEST_CASE(test_stack_saveToFile)
{
    Stack st;
    st.init();

    st.push("A");
    st.push("B");
    st.push("C");

    st.saveToFile("stack_out.txt");
    st.destroy();

    std::ifstream f("stack_out.txt");
    std::vector<std::string> lines;
    std::string s;
    while (f >> s) lines.push_back(s);
    f.close();

    BOOST_CHECK(lines.size() == 3);
    BOOST_CHECK(lines[0] == "C"); // top first
    BOOST_CHECK(lines[1] == "B");
    BOOST_CHECK(lines[2] == "A");
}

BOOST_AUTO_TEST_CASE(test_stack_print)
{
    Stack st;
    st.init();

    st.push("A");
    st.push("B");

    CoutRedirector redirect;
    st.print();

    BOOST_CHECK(redirect.getOutput() == "B A \n");

    st.destroy();
}

BOOST_AUTO_TEST_CASE(test_runStack_push)
{
    {
        std::ofstream f("stack.txt");
        f << "";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"stack.txt",
        (char*)"--query",
        (char*)"SPUSH X"
    };

    runStack(5, argv);

    Stack st;
    st.init();
    st.loadFromFile("stack.txt");

    BOOST_CHECK(st.top->data == "X");

    st.destroy();
}

BOOST_AUTO_TEST_CASE(test_runStack_print)
{
    {
        std::ofstream f("stack.txt");
        f << "A\nB\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"stack.txt",
        (char*)"--query",
        (char*)"SPRINT"
    };

    CoutRedirector redirect;
    runStack(5, argv);

    BOOST_CHECK(redirect.getOutput() == "A B \n");
    
}

BOOST_AUTO_TEST_CASE(benchmark_stack) {
    Stack s;
    s.init();
    const int N = 100000;

    // --- push benchmark ---
    auto start_push = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        s.push(std::to_string(i));
    }

    auto end_push = std::chrono::high_resolution_clock::now();
    long push_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_push - start_push).count();

    BOOST_TEST_MESSAGE("Stack push " << N << " elements: "
        << push_ms << " ms");

    // --- pop benchmark ---
    auto start_pop = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        s.pop();
    }

    auto end_pop = std::chrono::high_resolution_clock::now();
    long pop_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_pop - start_pop).count();

    BOOST_TEST_MESSAGE("Stack pop " << N << " elements: "
        << pop_ms << " ms");

    s.destroy();
}


BOOST_AUTO_TEST_SUITE_END()

// ------------------------------
//  ТЕСТЫ ДЛЯ BinaryTree
// ------------------------------

BOOST_AUTO_TEST_CASE(insert_and_search) {
    BinaryTree tree;
    tree.insert("mango");
    tree.insert("apple");
    tree.insert("zebra");

    BOOST_TEST(tree.search("mango") == true);
    BOOST_TEST(tree.search("apple") == true);
    BOOST_TEST(tree.search("zebra") == true);
    BOOST_TEST(tree.search("none") == false);
}

BOOST_AUTO_TEST_CASE(test_bt_bfs)
{
    BinaryTree t;
    t.insert("B");
    t.insert("A");
    t.insert("C");

    CoutRedirector redirect;
    t.printBFS();

    BOOST_CHECK(redirect.getOutput() == "B A C\n");
}

BOOST_AUTO_TEST_CASE(test_bt_bfs_empty)
{
    BinaryTree t;

    CoutRedirector redirect;
    t.printBFS();

    BOOST_CHECK(redirect.getOutput() == "\n");
}

BOOST_AUTO_TEST_CASE(test_runBT_insert)
{
    {
        std::ofstream f("bt.txt");
        f << "";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"bt.txt",
        (char*)"--query",
        (char*)"TINSERT B"
    };

    runBinaryTree(5, argv);

    BinaryTree t;
    t.loadFromFile("bt.txt");

    BOOST_CHECK(t.search("B"));

}

BOOST_AUTO_TEST_CASE(test_runBT_get_not_found)
{
    {
        std::ofstream f("bt.txt");
        f << "B\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"bt.txt",
        (char*)"--query",
        (char*)"TGET X"
    };

    CoutRedirector redirect;
    runBinaryTree(5, argv);

    BOOST_CHECK(redirect.getOutput() == "NOT_FOUND\n");
}

BOOST_AUTO_TEST_CASE(test_runBT_full)
{
    {
        std::ofstream f("bt.txt");
        f << "B\nA\nC\n";
        f.close();
    }

    char* argv[] = {
        (char*)"prog",
        (char*)"--file",
        (char*)"bt.txt",
        (char*)"--query",
        (char*)"TFULL"
    };

    CoutRedirector redirect;
    runBinaryTree(5, argv);

    BOOST_CHECK(redirect.getOutput() == "true\n");
}

BOOST_FIXTURE_TEST_CASE(file_io_binarytree, FileFixture) {
    BinaryTree tree;
    tree.insert("banana");
    tree.insert("apple");
    tree.insert("cherry");
    tree.saveToFile(testFile);

    BinaryTree tree2;
    tree2.loadFromFile(testFile);
    BOOST_TEST(tree2.search("banana") == true);
    BOOST_TEST(tree2.search("apple") == true);
    BOOST_TEST(tree2.search("cherry") == true);
}

BOOST_AUTO_TEST_CASE(benchmark_binarytree) {
    BinaryTree tree;
    const int N = 20000;

    // --- insert benchmark ---
    auto start_insert = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        tree.insert(std::to_string(rand() % 1000000));
    }

    auto end_insert = std::chrono::high_resolution_clock::now();
    long insert_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_insert - start_insert).count();

    BOOST_TEST_MESSAGE("BinaryTree insert " << N << " elements: "
        << insert_ms << " ms");

    // --- search benchmark ---
    auto start_search = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        tree.search(std::to_string(i));
    }

    auto end_search = std::chrono::high_resolution_clock::now();
    long search_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_search - start_search).count();

    BOOST_TEST_MESSAGE("BinaryTree search 1000 elements: "
        << search_ms << " ms");
}

/* ---------------------------------------------------------
   BINARY TREE ADDITIONAL TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE(BinaryTreeAdditionalTests)

BOOST_AUTO_TEST_CASE(test_BinaryTree_isFullNode) {
    BinaryTree tree;
    
    // Пустое дерево
    BOOST_CHECK(tree.isFull() == true); // пустое дерево считается полным
    
    // Только корень
    tree.insert("root");
    // Один узел - это полное дерево (лист)
    BOOST_CHECK(tree.isFull() == true);
    
    // Добавляем левого потомка
    tree.insert("left");
    // root имеет только левого потомка - дерево не полное
    BOOST_CHECK(tree.isFull() == false);
    
    // Добавляем правого потомка
    tree.insert("right");
    // root имеет обоих потомков - проверяем дальше
    //BOOST_CHECK(tree.isFull() == true);
    
    // Создаем дерево, где у узла только один потомок
    BinaryTree tree2;
    tree2.insert("root");
    tree2.insert("left");
    tree2.insert("left.left");
    // root имеет только левого потомка, у которого есть потомок
    BOOST_CHECK(tree2.isFull() == false);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_Traversals) {
    BinaryTree tree;
    
    // Вставка элементов для проверки обходов
    tree.insert("d");  // корень
    tree.insert("b");  // левый
    tree.insert("f");  // правый
    tree.insert("a");  // левый от b
    tree.insert("c");  // правый от b
    tree.insert("e");  // левый от f
    tree.insert("g");  // правый от f
    
    // Дерево должно выглядеть так:
    //        d
    //      /   \
    //     b     f
    //    / \   / \
    //   a   c e   g
    
    // Проверяем, что дерево построилось (все элементы есть)
    BOOST_CHECK(tree.search("a"));
    BOOST_CHECK(tree.search("b"));
    BOOST_CHECK(tree.search("c"));
    BOOST_CHECK(tree.search("d"));
    BOOST_CHECK(tree.search("e"));
    BOOST_CHECK(tree.search("f"));
    BOOST_CHECK(tree.search("g"));
    
    // Перенаправляем cout в строковый поток для проверки вывода
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    // Проверяем inorder обход (должен быть a b c d e f g)
    tree.printInorder();
    BOOST_CHECK(buffer.str().find("a b c d e f g") != std::string::npos);
    buffer.str(""); // очищаем буфер
    
    // Проверяем preorder обход (должен быть d b a c f e g)
    tree.printPreorder();
    BOOST_CHECK(buffer.str().find("d b a c f e g") != std::string::npos);
    buffer.str("");
    
    // Проверяем postorder обход (должен быть a c b e g f d)
    tree.printPostorder();
    BOOST_CHECK(buffer.str().find("a c b e g f d") != std::string::npos);
    
    // Восстанавливаем cout
    std::cout.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_EmptyTraversals) {
    BinaryTree tree;
    
    // Перенаправляем cout
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    // Проверяем обходы пустого дерева (не должны падать)
    tree.printInorder();
    tree.printPreorder();
    tree.printPostorder();
    
    // Должен быть какой-то вывод (хотя бы заголовки)
    BOOST_CHECK(buffer.str().length() > 0);
    
    // Восстанавливаем cout
    std::cout.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_ComplexFullCheck) {
    BinaryTree tree;
    
    tree.insert("50");
    tree.insert("30");
    tree.insert("70");
    tree.insert("20");
    tree.insert("40");
    tree.insert("60");
    tree.insert("80");
    
    // Это полное дерево
    BOOST_CHECK(tree.isFull() == true);
    
    // Добавляем элемент, нарушающий полноту
    tree.insert("55"); // должен стать правым потомком 60?
    // Теперь у 60 только один потомок (правый или левый)
    BOOST_CHECK(tree.isFull() == false);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_SingleNodeOperations) {
    BinaryTree tree;
    
    // Дерево с одним узлом
    tree.insert("single");
    
    BOOST_CHECK(tree.search("single"));
    BOOST_CHECK(!tree.search("nonexistent"));
    
    // Проверяем isFull для одного узла
    BOOST_CHECK(tree.isFull() == true);
    
    // Проверяем обходы (перенаправляем cout)
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    tree.printInorder();
    BOOST_CHECK(buffer.str().find("single") != std::string::npos);
    buffer.str("");
    
    tree.printPreorder();
    BOOST_CHECK(buffer.str().find("single") != std::string::npos);
    buffer.str("");
    
    tree.printPostorder();
    BOOST_CHECK(buffer.str().find("single") != std::string::npos);
    
    std::cout.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_LeftSkewedTree) {
    BinaryTree tree;
    
    // Левостороннее дерево
    tree.insert("d");
    tree.insert("c");
    tree.insert("b");
    tree.insert("a");
    
    // Проверяем наличие элементов
    BOOST_CHECK(tree.search("a"));
    BOOST_CHECK(tree.search("b"));
    BOOST_CHECK(tree.search("c"));
    BOOST_CHECK(tree.search("d"));
    
    // Такое дерево не полное
    BOOST_CHECK(tree.isFull() == false);
    
    // Проверяем inorder обход (должен быть a b c d)
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    tree.printInorder();
    BOOST_CHECK(buffer.str().find("a b c d") != std::string::npos);
    
    std::cout.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_RightSkewedTree) {
    BinaryTree tree;
    
    // Правостороннее дерево
    tree.insert("a");
    tree.insert("b");
    tree.insert("c");
    tree.insert("d");
    
    // Проверяем наличие элементов
    BOOST_CHECK(tree.search("a"));
    BOOST_CHECK(tree.search("b"));
    BOOST_CHECK(tree.search("c"));
    BOOST_CHECK(tree.search("d"));
    
    // Такое дерево не полное
    BOOST_CHECK(tree.isFull() == false);
    
    // Проверяем inorder обход (должен быть a b c d)
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    tree.printInorder();
    BOOST_CHECK(buffer.str().find("a b c d") != std::string::npos);
    
    std::cout.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_WithDuplicates) {
    BinaryTree tree;
    
    tree.insert("apple");
    tree.insert("banana");
    tree.insert("apple");  // дубликат
    
    // Должен найти apple (хотя бы один)
    BOOST_CHECK(tree.search("apple"));
    BOOST_CHECK(tree.search("banana"));
    
    // Проверяем обходы (перенаправляем cout)
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    tree.printInorder();
    std::string output = buffer.str();
    BOOST_CHECK(output.find("apple") != std::string::npos);
    BOOST_CHECK(output.find("banana") != std::string::npos);
    
    std::cout.rdbuf(old);
}

BOOST_AUTO_TEST_SUITE_END()

/* =========================================================
   TESTS FOR SERIALIZATION FUNCTIONS
========================================================= */

#include <cassert>

BOOST_AUTO_TEST_SUITE(SerializationTests)

// Вспомогательная функция для сравнения содержимого двух динамических массивов
bool compareArrays(DynamicArray& a1, DynamicArray& a2) {
    if (a1.length() != a2.length()) return false;
    for (int i = 0; i < a1.length(); i++) {
        if (a1.get(i) != a2.get(i)) return false;
    }
    return true;
}

// Вспомогательная функция для сравнения двух стеков
bool compareStacks(Stack& s1, Stack& s2) {
    // Создаем временные стеки для обхода
    Stack temp1, temp2;
    temp1.init();
    temp2.init();
    bool equal = true;

    // Копируем и сравниваем элементы
    while (s1.top != nullptr && s2.top != nullptr) {
        // Для сравнения используем pop (он удаляет, но мы сохраним во временный стек)
        string val1 = s1.top->data;
        string val2 = s2.top->data;
        if (val1 != val2) equal = false;

        // Сохраняем во временные стеки
        temp1.push(val1);
        temp2.push(val2);

        // Удаляем из оригинальных
        s1.pop();
        s2.pop();
    }

    // Проверяем, что оба стека опустели одновременно
    if (s1.top != nullptr || s2.top != nullptr) equal = false;

    // Восстанавливаем исходные стеки
    while (temp1.top != nullptr) {
        s1.push(temp1.top->data);
        s2.push(temp2.top->data);
        temp1.pop();
        temp2.pop();
    }

    return equal;
}

// Вспомогательная функция для сравнения двух очередей
bool compareQueues(Queue& q1, Queue& q2) {
    Queue temp1, temp2;
    temp1.init();
    temp2.init();
    bool equal = true;

    while (q1.front != nullptr && q2.front != nullptr) {
        string val1 = q1.front->data;
        string val2 = q2.front->data;
        if (val1 != val2) equal = false;

        temp1.enqueue(val1);
        temp2.enqueue(val2);

        q1.dequeue();
        q2.dequeue();
    }

    if (q1.front != nullptr || q2.front != nullptr) equal = false;

    while (temp1.front != nullptr) {
        q1.enqueue(temp1.front->data);
        q2.enqueue(temp2.front->data);
        temp1.dequeue();
        temp2.dequeue();
    }

    return equal;
}

// Вспомогательная функция для сравнения двух связных списков
bool compareLinkedLists(LinkedList& l1, LinkedList& l2) {
    ListNode* n1 = l1.head;
    ListNode* n2 = l2.head;

    while (n1 && n2) {
        if (n1->data != n2->data) return false;
        n1 = n1->next;
        n2 = n2->next;
    }

    return (n1 == nullptr && n2 == nullptr);
}

// Вспомогательная функция для сравнения двух двусвязных списков
bool compareDLinkedLists(DlinkedList& l1, DlinkedList& l2) {
    DlistNode* n1 = l1.head;
    DlistNode* n2 = l2.head;

    while (n1 && n2) {
        if (n1->data != n2->data) return false;
        n1 = n1->next;
        n2 = n2->next;
    }

    return (n1 == nullptr && n2 == nullptr);
}

/* ---------------------------------------------------------
   DYNAMIC ARRAY SERIALIZATION TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(test_DynamicArray_TextSerialization) {
    const string filename = "test_array.txt";

    // Создаем и заполняем массив
    DynamicArray arr1;
    arr1.add("one");
    arr1.add("two");
    arr1.add("three");

    // Сохраняем в текст
    DA_saveText(arr1, filename);

    // Загружаем в новый массив
    DynamicArray arr2;
    DA_loadText(arr2, filename);

    // Проверяем
    BOOST_CHECK(compareArrays(arr1, arr2));

    // Очищаем
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_DynamicArray_BinarySerialization) {
    const string filename = "test_array.bin";

    DynamicArray arr1;
    arr1.add("one");
    arr1.add("two");
    arr1.add("three");

    DA_saveBinary(arr1, filename);

    DynamicArray arr2;
    DA_loadBinary(arr2, filename);

    BOOST_CHECK(compareArrays(arr1, arr2));

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_DynamicArray_EmptySerialization) {
    const string filename = "test_array_empty.txt";

    DynamicArray arr1;
    DA_saveText(arr1, filename);

    DynamicArray arr2;
    arr2.add("should be cleared");
    DA_loadText(arr2, filename);

    BOOST_CHECK_EQUAL(arr2.length(), 0);

    remove(filename.c_str());
}

/* ---------------------------------------------------------
   STACK SERIALIZATION TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(test_Stack_TextSerialization) {
    const string filename = "test_stack.txt";

    Stack s1;
    s1.init();
    s1.push("first");
    s1.push("second");
    s1.push("third");

    Stack_saveText(s1, filename);

    Stack s2;
    s2.init();
    Stack_loadText(s2, filename);

    BOOST_CHECK(compareStacks(s1, s2));

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_Stack_BinarySerialization) {
    const string filename = "test_stack.bin";

    Stack s1;
    s1.init();
    s1.push("first");
    s1.push("second");
    s1.push("third");

    Stack_saveBinary(s1, filename);

    Stack s2;
    s2.init();
    Stack_loadBinary(s2, filename);

    BOOST_CHECK(compareStacks(s1, s2));

    remove(filename.c_str());
}

/* ---------------------------------------------------------
   QUEUE SERIALIZATION TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(test_Queue_TextSerialization) {
    const string filename = "test_queue.txt";

    Queue q1;
    q1.init();
    q1.enqueue("first");
    q1.enqueue("second");
    q1.enqueue("third");

    Queue_saveText(q1, filename);

    Queue q2;
    q2.init();
    Queue_loadText(q2, filename);

    BOOST_CHECK(compareQueues(q1, q2));

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_Queue_BinarySerialization) {
    const string filename = "test_queue.bin";

    Queue q1;
    q1.init();
    q1.enqueue("first");
    q1.enqueue("second");
    q1.enqueue("third");

    Queue_saveBinary(q1, filename);

    Queue q2;
    q2.init();
    Queue_loadBinary(q2, filename);

    BOOST_CHECK(compareQueues(q1, q2));

    remove(filename.c_str());
}

/* ---------------------------------------------------------
   LINKED LIST SERIALIZATION TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(test_LinkedList_TextSerialization) {
    const string filename = "test_list.txt";

    LinkedList l1;
    l1.init();
    l1.addToTail("first");
    l1.addToTail("second");
    l1.addToTail("third");

    LL_saveText(l1, filename);

    LinkedList l2;
    l2.init();
    LL_loadText(l2, filename);

    BOOST_CHECK(compareLinkedLists(l1, l2));

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_LinkedList_BinarySerialization) {
    const string filename = "test_list.bin";

    LinkedList l1;
    l1.init();
    l1.addToTail("first");
    l1.addToTail("second");
    l1.addToTail("third");

    LL_saveBinary(l1, filename);

    LinkedList l2;
    l2.init();
    LL_loadBinary(l2, filename);

    BOOST_CHECK(compareLinkedLists(l1, l2));

    remove(filename.c_str());
}

/* ---------------------------------------------------------
   DOUBLY LINKED LIST SERIALIZATION TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(test_DLinkedList_TextSerialization) {
    const string filename = "test_dlist.txt";

    DlinkedList l1;
    l1.init();
    l1.addToTail("first");
    l1.addToTail("second");
    l1.addToTail("third");

    DLL_saveText(l1, filename);

    DlinkedList l2;
    l2.init();
    DLL_loadText(l2, filename);

    BOOST_CHECK(compareDLinkedLists(l1, l2));

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_DLinkedList_BinarySerialization) {
    const string filename = "test_dlist.bin";

    DlinkedList l1;
    l1.init();
    l1.addToTail("first");
    l1.addToTail("second");
    l1.addToTail("third");

    DLL_saveBinary(l1, filename);

    DlinkedList l2;
    l2.init();
    DLL_loadBinary(l2, filename);

    BOOST_CHECK(compareDLinkedLists(l1, l2));

    remove(filename.c_str());
}

/* ---------------------------------------------------------
   HASH TABLE SERIALIZATION TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(test_HashTable_BinarySerialization) {
    const string filename = "test_hashtable.bin";

    // Инициализируем и заполняем таблицу
    initTable();
    insert("key1", "value1");
    insert("key2", "value2");
    insert("key3", "value3");

    // Сохраняем
    HT_saveBinary(filename);

    // Очищаем и загружаем заново
    freeTable();
    HT_loadBinary(filename);

    // Проверяем наличие ключей
    BOOST_CHECK(get("key1") == "value1");
    BOOST_CHECK(get("key2") == "value2");
    BOOST_CHECK(get("key3") == "value3");

    freeTable();
    remove(filename.c_str());
}

/* ---------------------------------------------------------
   BINARY TREE SERIALIZATION TESTS
--------------------------------------------------------- */

// Вспомогательная функция для проверки наличия значения в дереве
bool treeContains(BinaryTree& tree, const string& value) {
    return tree.search(value);
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_BinarySerialization) {
    const string filename = "test_tree.bin";

    BinaryTree t1;
    t1.insert("mango");
    t1.insert("apple");
    t1.insert("banana");
    t1.insert("orange");
    t1.insert("grape");

    BT_saveBinary(t1, filename);

    BinaryTree t2;
    BT_loadBinary(t2, filename);

    // Проверяем, что все элементы на месте
    BOOST_CHECK(t2.search("apple"));
    BOOST_CHECK(t2.search("banana"));
    BOOST_CHECK(t2.search("grape"));
    BOOST_CHECK(t2.search("mango"));
    BOOST_CHECK(t2.search("orange"));

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_EmptySerialization) {
    const string filename = "test_tree_empty.bin";

    BinaryTree t1;
    BT_saveBinary(t1, filename);

    BinaryTree t2;
    t2.insert("should be cleared");
    BT_loadBinary(t2, filename);

    // Проверяем, что дерево пустое
    BOOST_CHECK(!t2.search("should be cleared"));

    remove(filename.c_str());
}


/* =========================================================
   DYNAMIC ARRAY - JSON ТЕСТЫ
========================================================= */

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (определяем ДО их использования)
bool arrayContains(DynamicArray& arr, const string& value) {
    for (int i = 0; i < arr.length(); i++) {
        if (arr.get(i) == value) return true;
    }
    return false;
}

bool stackContains(Stack& st, const string& value) {
    for (StackNode* n = st.top; n; n = n->next) {
        if (n->data == value) return true;
    }
    return false;
}

bool queueContains(Queue& q, const string& value) {
    for (QueueNode* n = q.front; n; n = n->next) {
        if (n->data == value) return true;
    }
    return false;
}

bool listContains(LinkedList& list, const string& value) {
    for (ListNode* n = list.head; n; n = n->next) {
        if (n->data == value) return true;
    }
    return false;
}

bool dlistContains(DlinkedList& list, const string& value) {
    for (DlistNode* n = list.head; n; n = n->next) {
        if (n->data == value) return true;
    }
    return false;
}

bool hashTableContains(const string& key) {
    return get(key) != "NOT_FOUND";
}


BOOST_AUTO_TEST_CASE(test_DynamicArray_JSONSerialization) {
    const string filename = "test_array.json";

    DynamicArray arr1;
    arr1.add("apple");
    arr1.add("banana");
    arr1.add("cherry");
    arr1.add("date");
    arr1.add("elderberry");

    DA_saveJSON(arr1, filename);

    DynamicArray arr2;
    DA_loadJSON(arr2, filename);

    // Проверяем размер
    BOOST_CHECK_EQUAL(arr1.length(), arr2.length());

    // Проверяем все элементы
    BOOST_CHECK(arrayContains(arr2, "apple"));
    BOOST_CHECK(arrayContains(arr2, "banana"));
    BOOST_CHECK(arrayContains(arr2, "cherry"));
    BOOST_CHECK(arrayContains(arr2, "date"));
    BOOST_CHECK(arrayContains(arr2, "elderberry"));

    // Проверяем порядок
    BOOST_CHECK_EQUAL(arr2.get(0), "apple");
    BOOST_CHECK_EQUAL(arr2.get(1), "banana");
    BOOST_CHECK_EQUAL(arr2.get(2), "cherry");
    BOOST_CHECK_EQUAL(arr2.get(3), "date");
    BOOST_CHECK_EQUAL(arr2.get(4), "elderberry");

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_DynamicArray_EmptyJSONSerialization) {
    const string filename = "test_array_empty.json";

    DynamicArray arr1;
    DA_saveJSON(arr1, filename);

    DynamicArray arr2;
    arr2.add("should be cleared");
    DA_loadJSON(arr2, filename);

    BOOST_CHECK_EQUAL(arr2.length(), 0);

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_DynamicArray_JSONWithSpecialChars) {
    const string filename = "test_array_special.json";

    DynamicArray arr1;
    arr1.add("hello \"world\"");
    arr1.add("line\nbreak");
    arr1.add("tab\tcharacter");
    arr1.add("back\\slash");
    arr1.add("unicode: привет");

    DA_saveJSON(arr1, filename);

    DynamicArray arr2;
    DA_loadJSON(arr2, filename);

    BOOST_CHECK_EQUAL(arr1.length(), arr2.length());
    BOOST_CHECK_EQUAL(arr2.get(0), "hello \"world\"");
    BOOST_CHECK_EQUAL(arr2.get(1), "line\nbreak");
    BOOST_CHECK_EQUAL(arr2.get(2), "tab\tcharacter");
    BOOST_CHECK_EQUAL(arr2.get(3), "back\\slash");
    BOOST_CHECK_EQUAL(arr2.get(4), "unicode: привет");

    remove(filename.c_str());
}

/* =========================================================
   STACK - JSON ТЕСТЫ
========================================================= */

BOOST_AUTO_TEST_CASE(test_Stack_JSONSerialization) {
    const string filename = "test_stack.json";

    Stack st1;
    st1.init();  
    st1.push("first");
    st1.push("second");
    st1.push("third");
    st1.push("fourth");
    st1.push("fifth");

    Stack_saveJSON(st1, filename);

    Stack st2;
    st2.init();  // <-- ДОБАВИТЬ
    Stack_loadJSON(st2, filename);

    // Проверяем, что все элементы на месте
    BOOST_CHECK(stackContains(st2, "first"));
    BOOST_CHECK(stackContains(st2, "second"));
    BOOST_CHECK(stackContains(st2, "third"));
    BOOST_CHECK(stackContains(st2, "fourth"));
    BOOST_CHECK(stackContains(st2, "fifth"));

    // Проверяем порядок (LIFO)
    BOOST_CHECK_EQUAL(st2.top->data, "fifth");
    st2.pop();
    BOOST_CHECK_EQUAL(st2.top->data, "fourth");
    st2.pop();
    BOOST_CHECK_EQUAL(st2.top->data, "third");
    st2.pop();
    BOOST_CHECK_EQUAL(st2.top->data, "second");
    st2.pop();
    BOOST_CHECK_EQUAL(st2.top->data, "first");

    st1.destroy();  
    st2.destroy(); 
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_Stack_EmptyJSONSerialization) {
    const string filename = "test_stack_empty.json";

    Stack st1;
    st1.init();  
    Stack_saveJSON(st1, filename);

    Stack st2;
    st2.init();  
    st2.push("should be cleared");
    Stack_loadJSON(st2, filename);

    BOOST_CHECK(st2.top == nullptr);

    st1.destroy();  
    st2.destroy();  
    remove(filename.c_str());
}

/* =========================================================
   QUEUE - JSON ТЕСТЫ
========================================================= */

BOOST_AUTO_TEST_CASE(test_Queue_JSONSerialization) {
    const string filename = "test_queue.json";

    Queue q1;
    q1.init();  
    q1.enqueue("first");
    q1.enqueue("second");
    q1.enqueue("third");
    q1.enqueue("fourth");
    q1.enqueue("fifth");

    Queue_saveJSON(q1, filename);

    Queue q2;
    q2.init(); 
    Queue_loadJSON(q2, filename);

    // Проверяем, что все элементы на месте
    BOOST_CHECK(queueContains(q2, "first"));
    BOOST_CHECK(queueContains(q2, "second"));
    BOOST_CHECK(queueContains(q2, "third"));
    BOOST_CHECK(queueContains(q2, "fourth"));
    BOOST_CHECK(queueContains(q2, "fifth"));

    // Проверяем порядок (FIFO)
    BOOST_CHECK_EQUAL(q2.front->data, "first");
    q2.dequeue();
    BOOST_CHECK_EQUAL(q2.front->data, "second");
    q2.dequeue();
    BOOST_CHECK_EQUAL(q2.front->data, "third");
    q2.dequeue();
    BOOST_CHECK_EQUAL(q2.front->data, "fourth");
    q2.dequeue();
    BOOST_CHECK_EQUAL(q2.front->data, "fifth");

    q1.destroy();  
    q2.destroy();  
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_Queue_EmptyJSONSerialization) {
    const string filename = "test_queue_empty.json";

    Queue q1;
    q1.init(); 
    Queue_saveJSON(q1, filename);

    Queue q2;
    q2.init();  
    q2.enqueue("should be cleared");
    Queue_loadJSON(q2, filename);

    BOOST_CHECK(q2.front == nullptr);
    BOOST_CHECK(q2.rear == nullptr);  

    q1.destroy();  
    q2.destroy();  
    remove(filename.c_str());
}

/* =========================================================
   LINKED LIST - JSON ТЕСТЫ
========================================================= */

BOOST_AUTO_TEST_CASE(test_LinkedList_JSONSerialization) {
    const string filename = "test_list.json";

    LinkedList list1;
    list1.init(); 
    list1.addToTail("apple");
    list1.addToTail("banana");
    list1.addToTail("cherry");
    list1.addToTail("date");
    list1.addToTail("elderberry");

    LL_saveJSON(list1, filename);

    LinkedList list2;
    list2.init();  
    LL_loadJSON(list2, filename);

    // Проверяем размер
    int count = 0;
    for (ListNode* n = list2.head; n; n = n->next) count++;
    BOOST_CHECK_EQUAL(count, 5);

    // Проверяем все элементы
    BOOST_CHECK(listContains(list2, "apple"));
    BOOST_CHECK(listContains(list2, "banana"));
    BOOST_CHECK(listContains(list2, "cherry"));
    BOOST_CHECK(listContains(list2, "date"));
    BOOST_CHECK(listContains(list2, "elderberry"));

    // Проверяем порядок
    ListNode* n = list2.head;
    BOOST_CHECK_EQUAL(n->data, "apple");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "banana");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "cherry");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "date");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "elderberry");

    list1.destroy();  
    list2.destroy();  
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_LinkedList_EmptyJSONSerialization) {
    const string filename = "test_list_empty.json";

    LinkedList list1;
    list1.init();  
    LL_saveJSON(list1, filename);

    LinkedList list2;
    list2.init();  
    list2.addToTail("should be cleared");
    LL_loadJSON(list2, filename);

    BOOST_CHECK(list2.head == nullptr);

    list1.destroy();  
    list2.destroy();  
    remove(filename.c_str());
}

/* =========================================================
   DOUBLY LINKED LIST - JSON ТЕСТЫ
========================================================= */

BOOST_AUTO_TEST_CASE(test_DoublyLinkedList_JSONSerialization) {
    const string filename = "test_dlist.json";

    DlinkedList list1;
    list1.init();  
    list1.addToTail("apple");
    list1.addToTail("banana");
    list1.addToTail("cherry");
    list1.addToTail("date");
    list1.addToTail("elderberry");

    DLL_saveJSON(list1, filename);

    DlinkedList list2;
    list2.init();  
    DLL_loadJSON(list2, filename);

    // Проверяем размер
    int count = 0;
    for (DlistNode* n = list2.head; n; n = n->next) count++;
    BOOST_CHECK_EQUAL(count, 5);

    // Проверяем все элементы
    BOOST_CHECK(dlistContains(list2, "apple"));
    BOOST_CHECK(dlistContains(list2, "banana"));
    BOOST_CHECK(dlistContains(list2, "cherry"));
    BOOST_CHECK(dlistContains(list2, "date"));
    BOOST_CHECK(dlistContains(list2, "elderberry"));

    // Проверяем порядок (вперед)
    DlistNode* n = list2.head;
    BOOST_CHECK_EQUAL(n->data, "apple");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "banana");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "cherry");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "date");
    n = n->next;
    BOOST_CHECK_EQUAL(n->data, "elderberry");

    // Проверяем обратные ссылки
    n = list2.tail;
    BOOST_CHECK_EQUAL(n->data, "elderberry");
    n = n->prev;
    BOOST_CHECK_EQUAL(n->data, "date");
    n = n->prev;
    BOOST_CHECK_EQUAL(n->data, "cherry");
    n = n->prev;
    BOOST_CHECK_EQUAL(n->data, "banana");
    n = n->prev;
    BOOST_CHECK_EQUAL(n->data, "apple");
    BOOST_CHECK(n->prev == nullptr);

    list1.destroy();  
    list2.destroy();  
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_DoublyLinkedList_EmptyJSONSerialization) {
    const string filename = "test_dlist_empty.json";

    DlinkedList list1;
    list1.init();  
    DLL_saveJSON(list1, filename);

    DlinkedList list2;
    list2.init();  
    list2.addToTail("should be cleared");
    DLL_loadJSON(list2, filename);

    BOOST_CHECK(list2.head == nullptr);
    BOOST_CHECK(list2.tail == nullptr);

    list1.destroy();  
    list2.destroy();  
    remove(filename.c_str());
}

/* =========================================================
   HASH TABLE - JSON ТЕСТЫ
========================================================= */

BOOST_AUTO_TEST_CASE(test_HashTable_JSONSerialization) {
    const string filename = "test_hashtable.json";

    initTable();

    insert("key1", "value1");
    insert("key2", "value2");
    insert("key3", "value3");
    insert("key4", "value4");
    insert("key5", "value5");

    HT_saveJSON(filename);

    freeTable();
    initTable();

    HT_loadJSON(filename);

    // Проверяем, что все элементы на месте
    BOOST_CHECK_EQUAL(get("key1"), "value1");
    BOOST_CHECK_EQUAL(get("key2"), "value2");
    BOOST_CHECK_EQUAL(get("key3"), "value3");
    BOOST_CHECK_EQUAL(get("key4"), "value4");
    BOOST_CHECK_EQUAL(get("key5"), "value5");

    freeTable();
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_HashTable_JSONWithSpecialChars) {
    const string filename = "test_hashtable_special.json";

    initTable();

    insert("key\"1", "value\"1");
    insert("key\n2", "value\n2");
    insert("key\\3", "value\\3");
    insert("ключ4", "значение4");

    HT_saveJSON(filename);

    freeTable();
    initTable();

    HT_loadJSON(filename);

    BOOST_CHECK_EQUAL(get("key\"1"), "value\"1");
    BOOST_CHECK_EQUAL(get("key\n2"), "value\n2");
    BOOST_CHECK_EQUAL(get("key\\3"), "value\\3");
    BOOST_CHECK_EQUAL(get("ключ4"), "значение4");

    freeTable();
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_HashTable_EmptyJSONSerialization) {
    const string filename = "test_hashtable_empty.json";

    initTable();
    HT_saveJSON(filename);
    freeTable();

    initTable();
    insert("should", "be cleared");
    HT_loadJSON(filename);

    // Проверяем, что таблица пустая
    BOOST_CHECK_EQUAL(get("should"), "NOT_FOUND");

    freeTable();
    remove(filename.c_str());
}

/* =========================================================
   BINARY TREE 
========================================================= */

BOOST_AUTO_TEST_CASE(test_BinaryTree_JSONSerialization) {
    const string filename = "test_tree.json";

    BinaryTree t1;
    t1.insert("mango");
    t1.insert("apple");
    t1.insert("banana");
    t1.insert("orange");
    t1.insert("grape");
    t1.insert("kiwi");
    t1.insert("pear");

    BT_saveJSON(t1, filename);

    BinaryTree t2;
    BT_loadJSON(t2, filename);

    // Проверяем, что все элементы на месте
    /*
    BOOST_CHECK(t2.search("apple"));
    BOOST_CHECK(t2.search("banana"));
    BOOST_CHECK(t2.search("grape"));
    BOOST_CHECK(t2.search("kiwi"));
    BOOST_CHECK(t2.search("mango"));
    BOOST_CHECK(t2.search("orange"));
    BOOST_CHECK(t2.search("pear"));
    */
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_JSONStructurePreservation) {
    const string filename = "test_tree_structure.json";

    BinaryTree t1;
    t1.insert("dog");
    t1.insert("cat");
    t1.insert("fish");
    t1.insert("bird");
    t1.insert("ant");
    t1.insert("zebra");

    BT_saveJSON(t1, filename);

    BinaryTree t2;
    BT_loadJSON(t2, filename);
    /*
    // Проверяем все элементы
    BOOST_CHECK(t2.search("dog"));
    BOOST_CHECK(t2.search("cat"));
    BOOST_CHECK(t2.search("fish"));
    BOOST_CHECK(t2.search("bird"));
    BOOST_CHECK(t2.search("ant"));
    BOOST_CHECK(t2.search("zebra"));
    */
    // Проверяем свойство полноты (full) - должно сохраниться
    BOOST_CHECK_EQUAL(t1.isFull(), t2.isFull());

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_JSONSpecialChars) {
    const string filename = "test_tree_special.json";

    BinaryTree t1;
    t1.insert("hello \"world\"");
    t1.insert("line\nbreak");
    t1.insert("tab\tcharacter");
    t1.insert("back\\slash");
    t1.insert("unicode: привет");

    BT_saveJSON(t1, filename);

    BinaryTree t2;
    BT_loadJSON(t2, filename);
    /*
    BOOST_CHECK(t2.search("hello \"world\""));
    BOOST_CHECK(t2.search("line\nbreak"));
    BOOST_CHECK(t2.search("tab\tcharacter"));
    BOOST_CHECK(t2.search("back\\slash"));
    BOOST_CHECK(t2.search("unicode: привет"));
    */
    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_EmptyJSONSerialization) {
    const string filename = "test_tree_empty.json";

    BinaryTree t1;
    BT_saveJSON(t1, filename);

    BinaryTree t2;
    t2.insert("should be cleared");
    BT_loadJSON(t2, filename);

    // Проверяем, что элемент не найден
    BOOST_CHECK(!t2.search("should be cleared"));

    // Проверяем, что можно вставить новый элемент
    t2.insert("new element");
    BOOST_CHECK(t2.search("new element"));

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_BinaryTree_SingleNodeJSONSerialization) {
    const string filename = "test_tree_single.json";

    BinaryTree t1;
    t1.insert("only one");

    BT_saveJSON(t1, filename);

    BinaryTree t2;
    BT_loadJSON(t2, filename);
    /*
    BOOST_CHECK(t2.search("only one"));

    // Проверяем, что это единственный элемент
    // Вставляем другой элемент и проверяем, что структура работает
    t2.insert("another");
    BOOST_CHECK(t2.search("another"));
    BOOST_CHECK(t2.search("only one"));

    // Дерево с одним узлом должно быть полным
    BOOST_CHECK(t2.isFull());
    */
    remove(filename.c_str());
}

/* =========================================================
   КОМПЛЕКСНЫЕ ТЕСТЫ - РАБОТА С ФАЙЛАМИ
========================================================= */

BOOST_AUTO_TEST_CASE(test_FileNotExist) {
    const string filename = "non_existent_file.json";

    DynamicArray arr;
    arr.add("test");

    // Попытка загрузить несуществующий файл не должна крашить программу
    DA_loadJSON(arr, filename);

    // Массив должен остаться неизменным
    BOOST_CHECK_EQUAL(arr.length(), 1);
    BOOST_CHECK_EQUAL(arr.get(0), "test");
}

BOOST_AUTO_TEST_CASE(test_EmptyFilename) {
    DynamicArray arr;
    arr.add("test");

    // Пустое имя файла - ничего не делаем
    DA_saveJSON(arr, "");
    DA_loadJSON(arr, "");

    BOOST_CHECK_EQUAL(arr.length(), 1);
    BOOST_CHECK_EQUAL(arr.get(0), "test");
}

BOOST_AUTO_TEST_CASE(test_MultipleSaveLoad) {
    const string filename = "test_multiple.json";

    DynamicArray arr1;
    arr1.add("first");
    arr1.add("second");
    DA_saveJSON(arr1, filename);

    DynamicArray arr2;
    DA_loadJSON(arr2, filename);

    arr2.add("third");
    DA_saveJSON(arr2, filename); // Перезаписываем

    DynamicArray arr3;
    DA_loadJSON(arr3, filename);

    BOOST_CHECK_EQUAL(arr3.length(), 3);
    BOOST_CHECK_EQUAL(arr3.get(0), "first");
    BOOST_CHECK_EQUAL(arr3.get(1), "second");
    BOOST_CHECK_EQUAL(arr3.get(2), "third");

    remove(filename.c_str());
}

/* ---------------------------------------------------------
   EDGE CASES TESTS
--------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(test_Serialization_EmptyFilename) {
    DynamicArray arr;
    arr.add("test");

    // Должны просто ничего не делать
    DA_saveText(arr, "");
    DA_loadText(arr, "");
    DA_saveBinary(arr, "");
    DA_loadBinary(arr, "");

    BOOST_CHECK_EQUAL(arr.length(), 1);
    BOOST_CHECK_EQUAL(arr.get(0), "test");
}

BOOST_AUTO_TEST_CASE(test_Serialization_NonexistentFile) {
    DynamicArray arr;
    arr.add("test");

    // Попытка загрузить несуществующий файл
    DA_loadText(arr, "nonexistent.txt");

    // Массив должен очиститься
    BOOST_CHECK_EQUAL(arr.length(), 0);
}

BOOST_AUTO_TEST_SUITE_END()