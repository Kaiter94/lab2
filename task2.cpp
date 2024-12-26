#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class Set {
private:
    struct Node {
        string value;
        Node* next;
        Node(const string& v) : value(v), next(nullptr) {}
    };

    static const size_t TABLE_SIZE = 103;
    Node* table[TABLE_SIZE];

    size_t hashFunc(const string& s) const {
        size_t hashValue = 0;
        size_t base = 31;
        for (unsigned char c : s) {
            hashValue = (hashValue * base + c) % TABLE_SIZE;
        }
        return hashValue;
    }

public:
    Set() {
        for (size_t i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    ~Set() {
        for (size_t i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }

    void SETADD(const string& value) {
        size_t hashValue = hashFunc(value);
        Node* current = table[hashValue];
        while (current) {
            if (current->value == value) {
                return;
            }
            current = current->next;
        }
        Node* newNode = new Node(value);
        newNode->next = table[hashValue];
        table[hashValue] = newNode;
    }

    void SETDEL(const string& value) {
        size_t hashValue = hashFunc(value);
        Node* current = table[hashValue];
        Node* prev = nullptr;
        while (current) {
            if (current->value == value) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    table[hashValue] = current->next;
                }
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        }
    }

    bool SET_AT(const string& value) const {
        size_t hashValue = hashFunc(value);
        Node* current = table[hashValue];
        while (current) {
            if (current->value == value) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void PRINT() const {
        cout << "Множество: ";
        for (size_t i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current) {
                cout << current->value << " ";
                current = current->next;
            }
        }
        cout << endl;
    }

    void saveToJson(const string& filename) {
        json j;
        for (size_t i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current) {
                j["set"].push_back(current->value);
                current = current->next;
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
            if (j.contains("set")) {
                for (auto &val : j["set"]) {
                    SETADD(val.get<string>());
                }
            }
        }
    }
};

void processCommand(Set& s, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "SETADD") {
        string value;
        ss >> value;
        s.SETADD(value);
    } else if (operation == "SETDEL") {
        string value;
        ss >> value;
        s.SETDEL(value);
    } else if (operation == "SET_AT") {
        string value;
        ss >> value;
        bool exists = s.SET_AT(value);
        cout << (exists ? "Элемент присутствует в множестве" : "Элемент отсутствует в множестве") << endl;
    } else if (operation == "PRINT") {
        s.PRINT();
    } else {
        cout << "Ошибка: неизвестная операция." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./set --file <имя_файла> --query '<команда>'\n";
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

    Set s;
    s.loadFromJson(filename);
    processCommand(s, filename, command);
    s.saveToJson(filename);

    return 0;
}

