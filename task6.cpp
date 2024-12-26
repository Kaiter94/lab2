#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class HashTable {
private:
    static const int TABLE_SIZE = 10;

    struct Node {
        string key;
        string value;
        Node* next;
        Node(const string& k, const string& v) : key(k), value(v), next(nullptr) {}
    };

    Node* table[TABLE_SIZE];

    int hash(const string& key) {
        int hashValue = 0;
        for (char c : key) {
            hashValue = (hashValue + c) % TABLE_SIZE;
        }
        return hashValue;
    }

    void countDescendantsRecursive(const string& key, int& count) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* temp = table[i];
            while (temp) {
                if (temp->value == key) {
                    count++;
                    countDescendantsRecursive(temp->key, count);
                }
                temp = temp->next;
            }
        }
    }

    void buildTree(int n, istream& input) {
        for (int i = 0; i < n - 1; ++i) {
            string child, parent;
            input >> child >> parent;
            HSET(child, parent);
            if (HGET(parent) == "Не найдено") {
                HSET(parent, "");
            }
        }
    }

public:
    HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* temp = table[i];
            while (temp) {
                Node* toDelete = temp;
                temp = temp->next;
                delete toDelete;
            }
        }
    }

    void HSET(const string& key, const string& value) {
        int index = hash(key);
        Node* temp = table[index];
        while (temp) {
            if (temp->key == key) {
                temp->value = value;
                return;
            }
            temp = temp->next;
        }
        Node* newNode = new Node(key, value);
        newNode->next = table[index];
        table[index] = newNode;
    }

    string HGET(const string& key) {
        int index = hash(key);
        Node* temp = table[index];
        while (temp) {
            if (temp->key == key) {
                return temp->value;
            }
            temp = temp->next;
        }
        return "Не найдено";
    }

    void HDEL(const string& key) {
        int index = hash(key);
        Node* temp = table[index];
        Node* prev = nullptr;
        while (temp && temp->key != key) {
            prev = temp;
            temp = temp->next;
        }
        if (!temp) return;
        if (prev) {
            prev->next = temp->next;
        } else {
            table[index] = temp->next;
        }
        delete temp;
    }

    void PRINT() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            cout << i << ": ";
            Node* temp = table[i];
            while (temp) {
                cout << "{" << temp->key << ": " << temp->value << "} ";
                temp = temp->next;
            }
            cout << endl;
        }
    }

    void countAndPrintDescendants() {
      string keys[100];
      int keyCount = 0;

      for (int i = 0; i < TABLE_SIZE; i++) {
          Node* temp = table[i];
          while (temp) {
              keys[keyCount++] = temp->key;
              temp = temp->next;
          }
      }

      for (int i = 0; i < keyCount - 1; i++) {
          for (int j = i + 1; j < keyCount; j++) {
              if (keys[i] > keys[j]) {
                  swap(keys[i], keys[j]);
              }
          }
      }

      for (int i = 0; i < keyCount; i++) {
          int count = 0;
          countDescendantsRecursive(keys[i], count);
          cout << keys[i] << " " << count << endl;
      }
  }


    void saveToJson(const string& filename) {
        json j;
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* temp = table[i];
            while (temp) {
                j[to_string(i)].push_back({{"key", temp->key}, {"value", temp->value}});
                temp = temp->next;
            }
        }
        ofstream file(filename);
        if (file.is_open()) {
            file << j.dump(4);
            file.close();
        }
    }

    void loadFromJson(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            json j;
            file >> j;
            file.close();
            for (const auto& [index, nodes] : j.items()) {
                for (const auto& node : nodes) {
                    HSET(node["key"], node["value"]);
                }
            }
        }
    }

    void BUILD_TREE(int n, istream& input) {
        buildTree(n, input);
    }
};

void processCommand(HashTable& hashTable, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "HSET") {
        string key, value;
        ss >> key >> value;
        hashTable.HSET(key, value);
    } else if (operation == "HGET") {
        string key;
        ss >> key;
        cout << hashTable.HGET(key) << endl;
    } else if (operation == "HDEL") {
        string key;
        ss >> key;
        hashTable.HDEL(key);
    } else if (operation == "PRINT") {
        hashTable.PRINT();
    } else if (operation == "COUNT_DESCENDANTS") {
        hashTable.countAndPrintDescendants();
    } else if (operation == "BUILD_TREE") {
        int n;
        ss >> n;
        hashTable.BUILD_TREE(n, cin);
    } else {
        cout << "Неизвестная команда: " << operation << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./hash_table --file <имя_файла> --query '<команда>'\n";
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

    HashTable hashTable;
    hashTable.loadFromJson(filename);
    processCommand(hashTable, filename, command);
    hashTable.saveToJson(filename);

    return 0;
}

