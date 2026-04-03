#pragma once

#include <string>
#include <fstream> 

class BinaryTree {
public:
    // Конструктор и деструктор
    BinaryTree();

    ~BinaryTree();

    // Основные операции
    void insert(const std::string& key);
    bool search(const std::string& key);

    // Работа с файлами
    void saveToFile(const std::string& fileName);
    void loadFromFile(const std::string& fileName);

    // Проверка, является ли дерево полным (full)
    bool isFull() const;

    // Обходы дерева
    void printInorder();
    void printPreorder();
    void printPostorder();
    void printBFS();
    bool isEmpty() const { return root == nullptr; }

private:
    struct Node {
        std::string key;
        Node* left;
        Node* right;
        Node(const std::string& k) : key(k), left(nullptr), right(nullptr) {}
    };

    friend void BT_saveBinary(BinaryTree& t, const std::string& file);
    friend void BT_loadBinary(BinaryTree& t, const std::string& file);
    friend BinaryTree::Node* BT_loadNode(std::ifstream& f);
    friend void BT_saveNode(BinaryTree::Node* n, std::ofstream& f);

    friend void BT_saveJSON(BinaryTree& t, const std::string& file);
    friend void BT_loadJSON(BinaryTree& t, const std::string& file);
    friend void BT_saveNodeJSON(Node* n, std::ofstream& f, int depth);
    friend Node* BT_parseJSONNode(const std::string& json, size_t& pos, int depth);


    Node* root;

    // Вспомогательные рекурсивные функции
    Node* insert(Node* node, const std::string& key);
    void deleteTree(Node* node);
    void saveNode(Node* node, std::ofstream& file);
    void loadNode(std::ifstream& file);
    Node* loadNodeRecursive(std::ifstream& file);  
    bool isFullNode(Node* node) const;
    void inorder(Node* node);
    void preorder(Node* node);
    void postorder(Node* node);
};

void runBinaryTree(int argc, char* argv[]);