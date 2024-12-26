#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

class Set {
private:
    struct Node {
        int value;
        Node* next;
        Node(int v) : value(v), next(nullptr) {}
    };

    static const size_t TABLE_SIZE = 103;
    Node* table[TABLE_SIZE];

    size_t hashFunc(int x) const {
        return (size_t)x % TABLE_SIZE;
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

    void SETADD(int value) {
        if (value <= 0) {
            // Натуральные числа должны быть > 0
            return;
        }
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

    void SETDEL(int value) {
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

    bool SET_AT(int value) const {
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
                    SETADD(val.get<int>());
                }
            }
        }
    }

    int collectElements(int* arr, int maxSize) const {
        int idx = 0;
        for (size_t i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current) {
                if (idx < maxSize) {
                    arr[idx++] = current->value;
                }
                current = current->next;
            }
        }
        return idx;
    }
};

void partitionAndPrint(int* arr, int size) {
    if (size == 0) {
        cout << "Множество пусто." << endl;
        return;
    }

    int totalSum = 0;
    for (int i = 0; i < size; i++) {
        totalSum += arr[i];
    }


    const int MAX_SUM = 100000; 
    if (totalSum > MAX_SUM) {
        cout << "Слишком большая сумма." << endl;
        return;
    }

    bool** dp = new bool*[size+1];
    for (int i = 0; i <= size; i++) {
        dp[i] = new bool[totalSum+1];
        for (int s = 0; s <= totalSum; s++) {
            dp[i][s] = false;
        }
    }
    
    dp[0][0] = true;

    for (int i = 1; i <= size; i++) {
        int val = arr[i-1];
        for (int s = 0; s <= totalSum; s++) {
            if (dp[i-1][s]) {
                dp[i][s] = true;
                if (s + val <= totalSum) {
                    dp[i][s + val] = true;
                }
            }
        }
    }

    int bestSum = 0;
    for (int s = totalSum / 2; s >= 0; s--) {
        if (dp[size][s]) {
            bestSum = s;
            break;
        }
    }

    int sumA = bestSum;
    int sumB = totalSum - sumA;
    int diff = (sumB - sumA);
    if (diff < 0) diff = -diff;

    bool* used = new bool[size];
    for (int i = 0; i < size; i++) {
        used[i] = false;
    }

    int w = sumA;
    for (int i = size; i > 0; i--) {
        if (!dp[i-1][w]) {
            used[i-1] = true;
            w -= arr[i-1];
        }
        if (w == 0) break;
    }

    cout << "Подмножество A: { ";
    for (int i = 0; i < size; i++) {
        if (used[i]) {
            cout << arr[i] << " ";
        }
    }
    cout << "}" << endl;

    cout << "Подмножество B: { ";
    for (int i = 0; i < size; i++) {
        if (!used[i]) {
            cout << arr[i] << " ";
        }
    }
    cout << "}" << endl;

    cout << "Разница сумм = " << diff << endl;

    for (int i = 0; i <= size; i++) {
        delete[] dp[i];
    }
    delete[] dp;
    delete[] used;
}

void processCommand(Set& s, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "SETADD") {
        int value;
        ss >> value;
        s.SETADD(value);
    } else if (operation == "SETDEL") {
        int value;
        ss >> value;
        s.SETDEL(value);
    } else if (operation == "SET_AT") {
        int value;
        ss >> value;
        bool exists = s.SET_AT(value);
        cout << (exists ? "Элемент присутствует в множестве" : "Элемент отсутствует в множестве") << endl;
    } else if (operation == "PRINT") {
        s.PRINT();
    } else if (operation == "PARTITION") {
        const int MAX_SIZE = 100000; 
        int* arr = new int[MAX_SIZE];
        int n = s.collectElements(arr, MAX_SIZE);
        partitionAndPrint(arr, n);
        delete[] arr;
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
