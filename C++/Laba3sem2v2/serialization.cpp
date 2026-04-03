#include <fstream>
#include <vector>
#include <string>

using namespace std;

#include "array.h"
#include "stack.h"
#include "queue.h"
#include "linkedList.h"
#include "dlinkedList.h"
#include "hashTable.h"
#include "binaryTree.h"

#include "serialization.h"

/* =========================================================
   DYNAMIC ARRAY
========================================================= */

void DA_saveText(DynamicArray& arr, const string& file) {
    if (file.empty()) return;
    ofstream f(file);
    for (int i = 0; i < arr.length(); i++)
        f << arr.get(i) << "\n";
    f.close();
}

void DA_loadText(DynamicArray& arr, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    string s;
    arr.clear();
    while (f >> s) arr.add(s);
    f.close();
}

void DA_saveBinary(DynamicArray& arr, const string& file) {
    if (file.empty()) return;
    ofstream f(file, ios::binary);
    int n = arr.length();
    f.write((char*)&n, sizeof(n));
    for (int i = 0; i < n; i++) {
        string s = arr.get(i);
        size_t len = s.size();
        f.write((char*)&len, sizeof(len));
        f.write(s.c_str(), len);
    }
    f.close();
}

void DA_loadBinary(DynamicArray& arr, const string& file) {
    if (file.empty()) return;
    ifstream f(file, ios::binary);
    arr.clear();
    int n;
    f.read((char*)&n, sizeof(n));
    for (int i = 0; i < n; i++) {
        size_t len;
        f.read((char*)&len, sizeof(len));
        string s(len, '\0');
        f.read(&s[0], len);
        arr.add(s);
    }
    f.close();
}

/* =========================================================
   STACK
========================================================= */

void Stack_saveText(Stack& st, const string& file) {
    if (file.empty()) return;
    ofstream f(file);
    for (StackNode* n = st.top; n; n = n->next)
        f << n->data << "\n";
    f.close();
}

void Stack_loadText(Stack& st, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    string s;
    st.destroy();
    vector<string> temp;
    while (f >> s) temp.push_back(s);
    for (int i = temp.size() - 1; i >= 0; i--)
        st.push(temp[i]);
    f.close();
}

void Stack_saveBinary(Stack& st, const string& file) {
    if (file.empty()) return;
    ofstream f(file, ios::binary);
    vector<string> v;
    for (StackNode* n = st.top; n; n = n->next)
        v.push_back(n->data);

    int count = v.size();
    f.write((char*)&count, sizeof(count));

    for (auto& s : v) {
        size_t len = s.size();
        f.write((char*)&len, sizeof(len));
        f.write(s.c_str(), len);
    }
    f.close();
}

void Stack_loadBinary(Stack& st, const string& file) {
    if (file.empty()) return;
    ifstream f(file, ios::binary);
    st.destroy();
    int count;
    f.read((char*)&count, sizeof(count));
    vector<string> v(count);
    for (int i = 0; i < count; i++) {
        size_t len;
        f.read((char*)&len, sizeof(len));
        v[i].resize(len);
        f.read(&v[i][0], len);
    }
    for (int i = count - 1; i >= 0; i--)
        st.push(v[i]);
    f.close();
}

/* =========================================================
   QUEUE
========================================================= */

void Queue_saveText(Queue& q, const string& file) {
    if (file.empty()) return;
    ofstream f(file);
    for (QueueNode* n = q.front; n; n = n->next)
        f << n->data << "\n";
    f.close();
}

void Queue_loadText(Queue& q, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    string s;
    q.destroy();
    while (f >> s) q.enqueue(s);
    f.close();
}

void Queue_saveBinary(Queue& q, const string& file) {
    if (file.empty()) return;
    ofstream f(file, ios::binary);
    vector<string> v;
    for (QueueNode* n = q.front; n; n = n->next)
        v.push_back(n->data);

    int count = v.size();
    f.write((char*)&count, sizeof(count));

    for (auto& s : v) {
        size_t len = s.size();
        f.write((char*)&len, sizeof(len));
        f.write(s.c_str(), len);
    }
    f.close();
}

void Queue_loadBinary(Queue& q, const string& file) {
    if (file.empty()) return;
    ifstream f(file, ios::binary);
    q.destroy();
    int count;
    f.read((char*)&count, sizeof(count));
    for (int i = 0; i < count; i++) {
        size_t len;
        f.read((char*)&len, sizeof(len));
        string s(len, '\0');
        f.read(&s[0], len);
        q.enqueue(s);
    }
    f.close();
}

/* =========================================================
   LINKED LIST
========================================================= */

void LL_saveText(LinkedList& list, const string& file) {
    if (file.empty()) return;
    ofstream f(file);
    for (ListNode* n = list.head; n; n = n->next)
        f << n->data << "\n";
    f.close();
}

void LL_loadText(LinkedList& list, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    string s;
    list.destroy();
    while (f >> s) list.addToTail(s);
    f.close();
}

void LL_saveBinary(LinkedList& list, const string& file) {
    if (file.empty()) return;
    ofstream f(file, ios::binary);
    vector<string> v;
    for (ListNode* n = list.head; n; n = n->next)
        v.push_back(n->data);

    int count = v.size();
    f.write((char*)&count, sizeof(count));

    for (auto& s : v) {
        size_t len = s.size();
        f.write((char*)&len, sizeof(len));
        f.write(s.c_str(), len);
    }
    f.close();
}

void LL_loadBinary(LinkedList& list, const string& file) {
    if (file.empty()) return;
    ifstream f(file, ios::binary);
    list.destroy();
    int count;
    f.read((char*)&count, sizeof(count));
    for (int i = 0; i < count; i++) {
        size_t len;
        f.read((char*)&len, sizeof(len));
        string s(len, '\0');
        f.read(&s[0], len);
        list.addToTail(s);
    }
    f.close();
}

/* =========================================================
   DOUBLY LINKED LIST
========================================================= */

void DLL_saveText(DlinkedList& list, const string& file) {
    if (file.empty()) return;
    ofstream f(file);
    for (DlistNode* n = list.head; n; n = n->next)
        f << n->data << "\n";
    f.close();
}

void DLL_loadText(DlinkedList& list, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    string s;
    list.destroy();
    while (f >> s) list.addToTail(s);
    f.close();
}

void DLL_saveBinary(DlinkedList& list, const string& file) {
    if (file.empty()) return;
    ofstream f(file, ios::binary);
    vector<string> v;
    for (DlistNode* n = list.head; n; n = n->next)
        v.push_back(n->data);

    int count = v.size();
    f.write((char*)&count, sizeof(count));

    for (auto& s : v) {
        size_t len = s.size();
        f.write((char*)&len, sizeof(len));
        f.write(s.c_str(), len);
    }
    f.close();
}

void DLL_loadBinary(DlinkedList& list, const string& file) {
    if (file.empty()) return;
    ifstream f(file, ios::binary);
    list.destroy();
    int count;
    f.read((char*)&count, sizeof(count));
    for (int i = 0; i < count; i++) {
        size_t len;
        f.read((char*)&len, sizeof(len));
        string s(len, '\0');
        f.read(&s[0], len);
        list.addToTail(s);
    }
    f.close();
}

/* =========================================================
   HASH TABLE
========================================================= */

void HT_saveBinary(const string& file) {
    if (file.empty()) return;
    ofstream f(file, ios::binary);

    vector<pair<string, string>> all;
    for (int i = 0; i < TABLE_SIZE; i++)
        for (HashNode* n = hashTable[i]; n; n = n->next)
            all.push_back({ n->key, n->value });

    int count = all.size();
    f.write((char*)&count, sizeof(count));

    for (auto& kv : all) {
        size_t k = kv.first.size();
        size_t v = kv.second.size();
        f.write((char*)&k, sizeof(k));
        f.write(kv.first.c_str(), k);
        f.write((char*)&v, sizeof(v));
        f.write(kv.second.c_str(), v);
    }
    f.close();
}

void HT_loadBinary(const string& file) {
    if (file.empty()) return;
    ifstream f(file, ios::binary);
    freeTable();
    initTable();

    int count;
    f.read((char*)&count, sizeof(count));

    for (int i = 0; i < count; i++) {
        size_t klen, vlen;
        f.read((char*)&klen, sizeof(klen));
        string key(klen, '\0');
        f.read(&key[0], klen);

        f.read((char*)&vlen, sizeof(vlen));
        string val(vlen, '\0');
        f.read(&val[0], vlen);

        insert(key, val);
    }
    f.close();
}

/* =========================================================
   BINARY TREE
========================================================= */

void BT_saveNode(BinaryTree::Node* n, ofstream& f) {
    bool exists = (n != nullptr);
    f.write((char*)&exists, sizeof(exists));
    if (!exists) return;

    size_t len = n->key.size();
    f.write((char*)&len, sizeof(len));
    f.write(n->key.c_str(), len);

    BT_saveNode(n->left, f);
    BT_saveNode(n->right, f);
}

BinaryTree::Node* BT_loadNode(ifstream& f) {
    bool exists;
    if (!f.read((char*)&exists, sizeof(exists))) return nullptr;
    if (!exists) return nullptr;

    size_t len;
    f.read((char*)&len, sizeof(len));
    if (f.fail()) return nullptr;

    string key(len, '\0');
    f.read(&key[0], len);
    if (f.fail()) return nullptr;

    BinaryTree::Node* n = new BinaryTree::Node(key);
    n->left = BT_loadNode(f);
    n->right = BT_loadNode(f);
    return n;
}

void BT_saveBinary(BinaryTree& t, const string& file) {
    if (file.empty()) return;
    ofstream f(file, ios::binary);
    BT_saveNode(t.root, f);
    f.close();
}

void BT_loadBinary(BinaryTree& t, const string& file) {
    if (file.empty()) return;
    ifstream f(file, ios::binary);

    // Очищаем дерево
    t.~BinaryTree();
    new (&t) BinaryTree();

    // Загружаем новое дерево
    t.root = BT_loadNode(f);
    f.close();
}

/* =========================================================
   JSON SERIALIZATION
========================================================= */

string jsonEscape(const string& s) {
    string result;
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        switch (c) {
        case '"': result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\b': result += "\\b"; break;
        case '\f': result += "\\f"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default:
            if (c >= 0 && c <= 0x1F) {
                char buf[8];
                // Используем snprintf вместо sprintf
                snprintf(buf, sizeof(buf), "\\u%04x", c);
                result += buf;
            }
            else {
                result += c;
            }
        }
    }
    return result;
}

string jsonUnescape(const string& s) {
    string result;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == '\\' && i + 1 < s.length()) {
            i++; // сразу идём на следующий символ после '\'
            switch (s[i]) {
            case '"': result += '"'; break;
            case '\\': result += '\\'; break;
            case '/': result += '/'; break;
            case 'b': result += '\b'; break;
            case 'f': result += '\f'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            default: result += s[i]; break; // теперь i уже увеличен
            }
        }
        else {
            result += s[i];
        }
    }
    return result;
}

string trim(const string& s) {
    if (s.empty()) return s;

    size_t start = 0;
    while (start < s.length() && (s[start] == ' ' || s[start] == '\t' ||
        s[start] == '\n' || s[start] == '\r')) {
        start++;
    }

    if (start == s.length()) return "";

    size_t end = s.length() - 1;
    while (end > start && (s[end] == ' ' || s[end] == '\t' ||
        s[end] == '\n' || s[end] == '\r')) {
        end--;
    }

    return s.substr(start, end - start + 1);
}

/* ---------------------------------------------------------
   DYNAMIC ARRAY - JSON
--------------------------------------------------------- */

void DA_saveJSON(DynamicArray& arr, const string& file) {
    if (file.empty()) return;
    ofstream f(file);

    f << "[\n";
    for (int i = 0; i < arr.length(); i++) {
        f << "  \"" << jsonEscape(arr.get(i)) << "\"";
        if (i < arr.length() - 1) f << ",";
        f << "\n";
    }
    f << "]\n";
    f.close();
}

void DA_loadJSON(DynamicArray& arr, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    if (!f.is_open()) return;

    arr.clear();

    string line;
    string currentValue;
    bool inString = false;
    bool escapeNext = false;

    while (getline(f, line)) {
        string trimmed = trim(line);
        if (trimmed.empty() || trimmed == "[" || trimmed == "]") continue;

        // Remove trailing comma if exists
        if (!trimmed.empty() && trimmed[trimmed.length() - 1] == ',') {
            trimmed = trimmed.substr(0, trimmed.length() - 1);
        }

        // Remove quotes and unescape
        if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
            trimmed = trimmed.substr(1, trimmed.length() - 2);
            trimmed = jsonUnescape(trimmed);
            arr.add(trimmed);
        }
    }

    f.close();
}

/* ---------------------------------------------------------
   STACK - JSON
--------------------------------------------------------- */

void Stack_saveJSON(Stack& st, const string& file) {
    if (file.empty()) return;
    ofstream f(file);

    // First, collect items in reverse order to preserve stack order in file
    DynamicArray temp;
    for (StackNode* n = st.top; n; n = n->next) {
        temp.add(n->data);
    }

    f << "[\n";
    for (int i = temp.length() - 1; i >= 0; i--) {
        f << "  \"" << jsonEscape(temp.get(i)) << "\"";
        if (i > 0) f << ",";
        f << "\n";
    }
    f << "]\n";
    f.close();
}

void Stack_loadJSON(Stack& st, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    if (!f.is_open()) return;

    st.destroy();

    string line;
    DynamicArray items;

    while (getline(f, line)) {
        string trimmed = trim(line);
        if (trimmed.empty() || trimmed == "[" || trimmed == "]") continue;

        // Remove trailing comma if exists
        if (!trimmed.empty() && trimmed[trimmed.length() - 1] == ',') {
            trimmed = trimmed.substr(0, trimmed.length() - 1);
        }

        // Remove quotes and unescape
        if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
            trimmed = trimmed.substr(1, trimmed.length() - 2);
            trimmed = jsonUnescape(trimmed);
            items.add(trimmed);
        }
    }

    // Push items onto stack (preserve order)
    for (int i = 0; i < items.length(); i++) {
        st.push(items.get(i));
    }

    f.close();
}

/* ---------------------------------------------------------
   QUEUE - JSON
--------------------------------------------------------- */

void Queue_saveJSON(Queue& q, const string& file) {
    if (file.empty()) return;
    ofstream f(file);

    f << "[\n";
    for (QueueNode* n = q.front; n; n = n->next) {
        f << "  \"" << jsonEscape(n->data) << "\"";
        if (n->next) f << ",";
        f << "\n";
    }
    f << "]\n";
    f.close();
}

void Queue_loadJSON(Queue& q, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    if (!f.is_open()) return;

    q.destroy();

    string line;

    while (getline(f, line)) {
        string trimmed = trim(line);
        if (trimmed.empty() || trimmed == "[" || trimmed == "]") continue;

        // Remove trailing comma if exists
        if (!trimmed.empty() && trimmed[trimmed.length() - 1] == ',') {
            trimmed = trimmed.substr(0, trimmed.length() - 1);
        }

        // Remove quotes and unescape
        if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
            trimmed = trimmed.substr(1, trimmed.length() - 2);
            trimmed = jsonUnescape(trimmed);
            q.enqueue(trimmed);
        }
    }

    f.close();
}

/* ---------------------------------------------------------
   LINKED LIST - JSON
--------------------------------------------------------- */

void LL_saveJSON(LinkedList& list, const string& file) {
    if (file.empty()) return;
    ofstream f(file);

    f << "[\n";
    for (ListNode* n = list.head; n; n = n->next) {
        f << "  \"" << jsonEscape(n->data) << "\"";
        if (n->next) f << ",";
        f << "\n";
    }
    f << "]\n";
    f.close();
}

void LL_loadJSON(LinkedList& list, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    if (!f.is_open()) return;

    list.destroy();

    string line;

    while (getline(f, line)) {
        string trimmed = trim(line);
        if (trimmed.empty() || trimmed == "[" || trimmed == "]") continue;

        // Remove trailing comma if exists
        if (!trimmed.empty() && trimmed[trimmed.length() - 1] == ',') {
            trimmed = trimmed.substr(0, trimmed.length() - 1);
        }

        // Remove quotes and unescape
        if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
            trimmed = trimmed.substr(1, trimmed.length() - 2);
            trimmed = jsonUnescape(trimmed);
            list.addToTail(trimmed);
        }
    }

    f.close();
}

/* ---------------------------------------------------------
   DOUBLY LINKED LIST - JSON
--------------------------------------------------------- */

void DLL_saveJSON(DlinkedList& list, const string& file) {
    if (file.empty()) return;
    ofstream f(file);

    f << "[\n";
    for (DlistNode* n = list.head; n; n = n->next) {
        f << "  \"" << jsonEscape(n->data) << "\"";
        if (n->next) f << ",";
        f << "\n";
    }
    f << "]\n";
    f.close();
}

void DLL_loadJSON(DlinkedList& list, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    if (!f.is_open()) return;

    list.destroy();

    string line;

    while (getline(f, line)) {
        string trimmed = trim(line);
        if (trimmed.empty() || trimmed == "[" || trimmed == "]") continue;

        // Remove trailing comma if exists
        if (!trimmed.empty() && trimmed[trimmed.length() - 1] == ',') {
            trimmed = trimmed.substr(0, trimmed.length() - 1);
        }

        // Remove quotes and unescape
        if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
            trimmed = trimmed.substr(1, trimmed.length() - 2);
            trimmed = jsonUnescape(trimmed);
            list.addToTail(trimmed);
        }
    }

    f.close();
}

/* ---------------------------------------------------------
   HASH TABLE - JSON
--------------------------------------------------------- */

void HT_saveJSON(const string& file) {
    if (file.empty()) return;
    ofstream f(file);

    f << "{\n";

    bool first = true;
    for (int i = 0; i < TABLE_SIZE; i++) {
        for (HashNode* n = hashTable[i]; n; n = n->next) {
            if (!first) {
                f << ",\n";
            }
            f << "  \"" << jsonEscape(n->key) << "\": \"" << jsonEscape(n->value) << "\"";
            first = false;
        }
    }

    f << "\n}\n";
    f.close();
}

void HT_loadJSON(const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    if (!f.is_open()) return;

    freeTable();
    initTable();

    string line;
    string key, value;
    bool readingKey = true;
    bool inString = false;
    bool escapeNext = false;

    while (getline(f, line)) {
        string trimmed = trim(line);
        if (trimmed.empty() || trimmed == "{" || trimmed == "}") continue;

        // Remove trailing comma if exists
        if (!trimmed.empty() && trimmed[trimmed.length() - 1] == ',') {
            trimmed = trimmed.substr(0, trimmed.length() - 1);
        }

        // Parse key-value pair
        size_t colonPos = trimmed.find(':');
        if (colonPos != string::npos) {
            string keyPart = trim(trimmed.substr(0, colonPos));
            string valuePart = trim(trimmed.substr(colonPos + 1));

            // Extract key (remove quotes)
            if (keyPart.length() >= 2 && keyPart[0] == '"' && keyPart[keyPart.length() - 1] == '"') {
                keyPart = keyPart.substr(1, keyPart.length() - 2);
                keyPart = jsonUnescape(keyPart);
            }

            // Extract value (remove quotes)
            if (valuePart.length() >= 2 && valuePart[0] == '"' && valuePart[valuePart.length() - 1] == '"') {
                valuePart = valuePart.substr(1, valuePart.length() - 2);
                valuePart = jsonUnescape(valuePart);
            }

            insert(keyPart, valuePart);
        }
    }

    f.close();
}

/* ---------------------------------------------------------
   BINARY TREE - JSON
--------------------------------------------------------- */

void BT_saveNodeJSON(BinaryTree::Node* n, ofstream& f, int depth) {
    if (!n) {
        for (int i = 0; i < depth; i++) f << "  ";
        f << "null";
        return;
    }

    for (int i = 0; i < depth; i++) f << "  ";
    f << "{\n";

    // Key
    for (int i = 0; i <= depth; i++) f << "  ";
    f << "\"key\": \"" << jsonEscape(n->key) << "\"";

    // Left child
    f << ",\n";
    for (int i = 0; i <= depth; i++) f << "  ";
    f << "\"left\": ";
    BT_saveNodeJSON(n->left, f, depth + 1);

    // Right child
    f << ",\n";
    for (int i = 0; i <= depth; i++) f << "  ";
    f << "\"right\": ";
    BT_saveNodeJSON(n->right, f, depth + 1);

    f << "\n";
    for (int i = 0; i < depth; i++) f << "  ";
    f << "}";
}

void BT_saveJSON(BinaryTree& t, const string& file) {
    if (file.empty()) return;
    ofstream f(file);

    if (t.root) {
        BT_saveNodeJSON(t.root, f, 0);
    }
    else {
        f << "null";
    }

    f.close();
}

BinaryTree::Node* BT_parseJSONNode(const string& json, size_t& pos, int depth) {
    // Skip whitespace
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' ||
        json[pos] == '\n' || json[pos] == '\r')) {
        pos++;
    }

    if (pos >= json.length()) return nullptr;

    // Check for null
    if (json.substr(pos, 4) == "null") {
        pos += 4;
        return nullptr;
    }

    // Expect '{'
    if (json[pos] != '{') return nullptr;
    pos++;

    // Skip whitespace
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' ||
        json[pos] == '\n' || json[pos] == '\r')) {
        pos++;
    }

    string key;
    BinaryTree::Node* left = nullptr;
    BinaryTree::Node* right = nullptr;

    // Parse key-value pairs
    while (pos < json.length() && json[pos] != '}') {
        // Skip whitespace
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' ||
            json[pos] == '\n' || json[pos] == '\r')) {
            pos++;
        }

        // Parse key
        if (json[pos] == '"') {
            pos++;
            key.clear();
            while (pos < json.length() && json[pos] != '"') {
                if (json[pos] == '\\' && pos + 1 < json.length()) {
                    key += json[pos];
                    key += json[pos + 1];
                    pos += 2;
                }
                else {
                    key += json[pos];
                    pos++;
                }
            }
            if (pos < json.length() && json[pos] == '"') pos++;

            // Skip whitespace
            while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' ||
                json[pos] == '\n' || json[pos] == '\r')) {
                pos++;
            }

            // Expect ':'
            if (pos < json.length() && json[pos] == ':') pos++;

            // Skip whitespace
            while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' ||
                json[pos] == '\n' || json[pos] == '\r')) {
                pos++;
            }

            // Parse value based on key
            string keyStr = jsonUnescape(key);
            if (keyStr == "key") {
                // Parse string value
                if (pos < json.length() && json[pos] == '"') {
                    pos++;
                    string val;
                    while (pos < json.length() && json[pos] != '"') {
                        if (json[pos] == '\\' && pos + 1 < json.length()) {
                            val += json[pos];
                            val += json[pos + 1];
                            pos += 2;
                        }
                        else {
                            val += json[pos];
                            pos++;
                        }
                    }
                    if (pos < json.length() && json[pos] == '"') pos++;
                    key = jsonUnescape(val);
                }
            }
            else if (keyStr == "left") {
                left = BT_parseJSONNode(json, pos, depth + 1);
            }
            else if (keyStr == "right") {
                right = BT_parseJSONNode(json, pos, depth + 1);
            }

            // Skip whitespace
            while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' ||
                json[pos] == '\n' || json[pos] == '\r')) {
                pos++;
            }

            // Check for comma
            if (pos < json.length() && json[pos] == ',') {
                pos++;
            }
        }
    }

    // Expect '}'
    if (pos < json.length() && json[pos] == '}') pos++;

    // Create node
    BinaryTree::Node* node = new BinaryTree::Node(key);
    node->left = left;
    node->right = right;
    return node;
}

void BT_loadJSON(BinaryTree& t, const string& file) {
    if (file.empty()) return;
    ifstream f(file);
    if (!f.is_open()) return;

    // Read entire file
    string json((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());

    // Destroy current tree
    t.~BinaryTree();
    new (&t) BinaryTree();

    // Parse JSON
    size_t pos = 0;
    t.root = BT_parseJSONNode(json, pos, 0);

    f.close();
}