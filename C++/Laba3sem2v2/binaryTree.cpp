#include "binaryTree.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

using namespace std;

BinaryTree::BinaryTree() : root(nullptr) {}

BinaryTree::~BinaryTree() {
    if (root) {
        deleteTree(root);
        root = nullptr;
    }
}

void BinaryTree::deleteTree(Node* node) {
    if (!node) return;
    // Сначала удаляем поддеревья, потом сам узел
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}


void BinaryTree::insert(const std::string& key) {
    root = insert(root, key);
}

BinaryTree::Node* BinaryTree::insert(Node* node, const std::string& key) {
    if (key.empty())
        return node;

    if (node == nullptr)
        return new Node(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);

    return node;
}

bool BinaryTree::search(const std::string& key) {
    Node* current = root;

    while (current != nullptr) {
        if (key == current->key)
            return true;
        else if (key < current->key)
            current = current->left;
        else
            current = current->right;
    }

    return false;
}

// ИСПРАВЛЕНО: бинарная запись с префиксным обходом
void BinaryTree::saveNode(Node* n, std::ofstream& f) {
    bool exists = (n != nullptr);
    f.write((char*)&exists, sizeof(exists));
    if (!exists) return;

    size_t len = n->key.size();
    f.write((char*)&len, sizeof(len));
    f.write(n->key.c_str(), len);

    saveNode(n->left, f);
    saveNode(n->right, f);
}

// ИСПРАВЛЕНО: бинарное чтение с префиксным обходом
void BinaryTree::loadNode(std::ifstream& f) {
    // Очищаем текущее дерево
    deleteTree(root);
    root = nullptr;

    // Рекурсивно загружаем узлы
    root = loadNodeRecursive(f);
}

// Добавляем новый вспомогательный метод для рекурсивной загрузки
BinaryTree::Node* BinaryTree::loadNodeRecursive(std::ifstream& f) {
    bool exists;
    if (!f.read((char*)&exists, sizeof(exists))) {
        return nullptr;
    }

    if (!exists) return nullptr;

    size_t len;
    f.read((char*)&len, sizeof(len));
    if (f.fail()) return nullptr;

    // Проверяем, что len разумная (не слишком большая)
    if (len > 10000) { // Защита от некорректных данных
        return nullptr;
    }

    string key(len, '\0');
    f.read(&key[0], len);
    if (f.fail()) return nullptr;

    Node* node = nullptr;
    try {
        node = new Node(key);
        node->left = loadNodeRecursive(f);
        node->right = loadNodeRecursive(f);
    }
    catch (const std::bad_alloc&) {
        // Если не хватает памяти, возвращаем nullptr
        return nullptr;
    }

    return node;
}

void BinaryTree::saveToFile(const string& fileName) {
    if (fileName.empty()) return; // ДОБАВЛЕНА ПРОВЕРКА
    ofstream file(fileName, ios::binary);  // ВАЖНО: бинарный режим
    if (!file.is_open()) {
        cout << "Не удалось открыть файл для записи." << endl;
        return;
    }
    saveNode(root, file);
    file.close();
}

void BinaryTree::loadFromFile(const string& fileName) {
    if (fileName.empty()) return;

    // Очищаем текущее дерево
    if (root) {
        deleteTree(root);
        root = nullptr;
    }

    ifstream file(fileName, ios::binary);
    if (!file.is_open()) {
        return;
    }

    root = loadNodeRecursive(file);
    file.close();
}

bool BinaryTree::isFullNode(Node* node) const {
    if (!node) return true;

    bool hasLeft = (node->left != nullptr);
    bool hasRight = (node->right != nullptr);

    // Если узел имеет ровно двух потомков
    if (hasLeft && hasRight) {
        return isFullNode(node->left) && isFullNode(node->right);
    }
    // Если узел не имеет потомков (лист)
    else if (!hasLeft && !hasRight) {
        return true;
    }
    // Если узел имеет ровно одного потомка
    else {
        return false;
    }
}

bool BinaryTree::isFull() const {
    if (!root) return true; // Пустое дерево считаем полным
    return isFullNode(root);
}


void BinaryTree::inorder(Node* node) {
    if (!node) return;
    inorder(node->left);
    cout << node->key << " ";
    inorder(node->right);
}

void BinaryTree::preorder(Node* node) {
    if (!node) return;
    cout << node->key << " ";
    preorder(node->left);
    preorder(node->right);
}

void BinaryTree::postorder(Node* node) {
    if (!node) return;
    postorder(node->left);
    postorder(node->right);
    cout << node->key << " ";
}

void BinaryTree::printInorder() {
    cout << "Inorder traversal: ";
    inorder(root);
    cout << endl;
}

void BinaryTree::printPreorder() {
    cout << "Preorder traversal: ";
    preorder(root);
    cout << endl;
}

void BinaryTree::printPostorder() {
    cout << "Postorder traversal: ";
    postorder(root);
    cout << endl;
}

void BinaryTree::printBFS() {
    if (!root) {
        cout << endl;
        return;
    }

    queue<Node*> q;
    q.push(root);
    bool first = true;

    while (!q.empty()) {
        Node* cur = q.front();
        q.pop();

        if (!first) cout << " ";
        cout << cur->key;
        first = false;

        if (cur->left) q.push(cur->left);
        if (cur->right) q.push(cur->right);
    }
    cout << endl;
}

void runBinaryTree(int argc, char* argv[]) {
    BinaryTree tree;

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

    // Загружаем дерево только если файл существует и не пустой
    if (!fileName.empty()) {
        // Проверяем, существует ли файл
        ifstream test(fileName);
        if (test.good()) {
            test.close();
            tree.loadFromFile(fileName);
        }
    }

    string command;
    string arg;
    size_t pos = query.find(' ');
    if (pos != string::npos) {
        command = query.substr(0, pos);
        arg = query.substr(pos + 1);
    }
    else {
        command = query;
    }

    if (command == "TINSERT") {
        if (!arg.empty()) {
            tree.insert(arg);
            if (!fileName.empty()) tree.saveToFile(fileName);
        }
    }
    else if (command == "TGET") {
        if (tree.search(arg)) cout << arg << endl;
        else cout << "NOT_FOUND" << endl;
    }
    else if (command == "TFULL") {
        cout << (tree.isFull() ? "true" : "false") << endl;
    }
    else if (command == "TSEARCH") {
        cout << (tree.search(arg) ? "true" : "false") << endl;
    }
    else if (command == "TINORDER") {
        tree.printInorder();
    }
    else if (command == "TPREORDER") {
        tree.printPreorder();
    }
    else if (command == "TPOSTORDER") {
        tree.printPostorder();
    }
    else if (command == "TBFS") {
        cout << "BFS обход: ";
        tree.printBFS();
    }
}