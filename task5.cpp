#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class BinaryTree {
private:
    struct Node {
        int key;
        Node* left;
        Node* right;
        Node(int k) : key(k), left(nullptr), right(nullptr) {}
    };

    Node* root;

    Node* insert(Node* node, int key) {
        if (!node) return new Node(key);
        if (key < node->key) {
            node->left = insert(node->left, key);
        } else if (key > node->key) {
            node->right = insert(node->right, key);
        }
        return node;
    }

    Node* deleteNode(Node* node, int key) {
        if (!node) return node;
        if (key < node->key) {
            node->left = deleteNode(node->left, key);
        } else if (key > node->key) {
            node->right = deleteNode(node->right, key);
        } else {
            if (!node->left) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                Node* temp = node->left;
                delete node;
                return temp;
            }
            Node* temp = minValueNode(node->right);
            node->key = temp->key;
            node->right = deleteNode(node->right, temp->key);
        }
        return node;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current && current->left) {
            current = current->left;
        }
        return current;
    }

    bool search(Node* node, int key) {
        if (!node) return false;
        if (key == node->key) return true;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }

    void toJson(Node* node, json& jNode) {
        if (!node) return;
        jNode["key"] = node->key;
        if (node->left) {
            jNode["left"] = json::object();
            toJson(node->left, jNode["left"]);
        }
        if (node->right) {
            jNode["right"] = json::object();
            toJson(node->right, jNode["right"]);
        }
    }

    Node* buildTreeFromJson(const json& jNode) {
        if (jNode.is_null()) return nullptr;
        Node* node = new Node(jNode["key"]);
        if (jNode.contains("left")) node->left = buildTreeFromJson(jNode["left"]);
        if (jNode.contains("right")) node->right = buildTreeFromJson(jNode["right"]);
        return node;
    }

    void printLeavesRecursive(Node* node) {
        if (!node) return;
        if (!node->left && !node->right) {
            cout << node->key << " ";
        }
        printLeavesRecursive(node->left);
        printLeavesRecursive(node->right);
    }

public:
    BinaryTree() : root(nullptr) {}

    void TINSERT(int key) {
        root = insert(root, key);
    }

    void TDEL(int key) {
        root = deleteNode(root, key);
    }

    bool TGET(int key) {
        return search(root, key);
    }

    void PRINT_LEAVES() {
        cout << "Листья дерева в порядке возрастания: ";
        printLeavesRecursive(root);
        cout << endl;
    }

    void saveToJson(const string& filename) {
        json treeJson;
        toJson(root, treeJson);
        ofstream file(filename);
        if (file.is_open()) {
            file << treeJson.dump(4);
            file.close();
        }
    }

    void loadFromJson(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            json treeJson;
            file >> treeJson;
            file.close();
            root = buildTreeFromJson(treeJson);
        }
    }
};

void processCommand(BinaryTree& tree, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "TINSERT") {
        int key;
        ss >> key;
        tree.TINSERT(key);
    } else if (operation == "TDEL") {
        int key;
        ss >> key;
        tree.TDEL(key);
    } else if (operation == "TGET") {
        int key;
        ss >> key;
        cout << (tree.TGET(key) ? "Найдено" : "Не найдено") << endl;
    } else if (operation == "PRINT_LEAVES") {
        tree.PRINT_LEAVES();
    } else if (operation == "PRINT") {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            cout << "Содержимое дерева: " << endl;
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        } else {
            cout << "Ошибка: невозможно открыть файл." << endl;
        }
    } else {
        cout << "Ошибка: неизвестная операция." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./tree --file <имя_файла> --query '<команда>'\n";
        return 1;
    }

    string fileArg = argv[1];
    string filename = argv[2];
    string queryArg = argv[3];
    string command = argv[4];

    if (fileArg != "--file" || queryArg != "--query") {
        cerr << "Неверные аргументы. Используйте --file и --query.\n";
        return 1;
    }

    BinaryTree tree;
    tree.loadFromJson(filename);
    processCommand(tree, filename, command);
    tree.saveToJson(filename);

    return 0;
}
