#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

class Array {
private:
    int* data;
    int capacity;
    int size;

    void resize() {
        capacity *= 2;
        int* newData = new int[capacity];
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    Array() : capacity(10), size(0) {
        data = new int[capacity];
    }

    ~Array() {
        delete[] data;
    }

    void MPUSH(int value) {
        if (size == capacity) {
            resize();
        }
        data[size++] = value;
    }

    void MPUSH_AT(int index, int value) {
        if (index < 0 || index > size) {
            cout << "Ошибка: индекс вне границ." << endl;
            return;
        }
        if (size == capacity) {
            resize();
        }
        for (int i = size; i > index; i--) {
            data[i] = data[i - 1];
        }
        data[index] = value;
        size++;
    }

    void MDEL(int index) {
        if (index < 0 || index >= size) {
            cout << "Ошибка: индекс вне границ." << endl;
            return;
        }
        for (int i = index; i < size - 1; i++) {
            data[i] = data[i + 1];
        }
        size--;
    }

    int MGET(int index) {
        if (index < 0 || index >= size) {
            cout << "Ошибка: индекс вне границ." << endl;
            return -1;
        }
        return data[index];
    }

    void MSET(int index, int value) {
        if (index < 0 || index >= size) {
            cout << "Ошибка: индекс вне границ." << endl;
            return;
        }
        data[index] = value;
    }

    int MLENGTH() {
        return size;
    }

    void PRINT() {
        cout << "Массив: ";
        for (int i = 0; i < size; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }

    void saveToJson(const string& filename) {
        json j;
        for (int i = 0; i < size; i++) {
            j["array"].push_back(data[i]);
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
            if (j.contains("array")) {
                for (const auto& val : j["array"]) {
                    MPUSH(val);
                }
            }
        }
    }

    void findSubarraysWithSum(int targetSum) {
        bool found = false;
        for (int start = 0; start < size; ++start) {
            int sum = 0;
            for (int end = start; end < size; ++end) {
                sum += data[end];
                if (sum == targetSum) {
                    found = true;
                    cout << "Подмассив: [";
                    for (int i = start; i <= end; ++i) {
                        cout << data[i];
                        if (i < end) cout << ", ";
                    }
                    cout << "]" << endl;
                }
            }
        }
        if (!found) {
            cout << "Нет подмассивов с суммой " << targetSum << "." << endl;
        }
    }
};

void processCommand(Array& array, const string& filename, const string& command) {
    stringstream ss(command);
    string operation;
    ss >> operation;

    if (operation == "MPUSH") {
        int value;
        ss >> value;
        array.MPUSH(value);
    } else if (operation == "MPUSH_AT") {
        int index, value;
        ss >> index >> value;
        array.MPUSH_AT(index, value);
    } else if (operation == "MDEL") {
        int index;
        ss >> index;
        array.MDEL(index);
    } else if (operation == "MGET") {
        int index;
        ss >> index;
        cout << "Элемент по индексу " << index << ": " << array.MGET(index) << endl;
    } else if (operation == "MSET") {
        int index, value;
        ss >> index >> value;
        array.MSET(index, value);
    } else if (operation == "MLENGTH") {
        cout << "Длина массива: " << array.MLENGTH() << endl;
    } else if (operation == "PRINT") {
        array.PRINT();
    } else if (operation == "FIND_SUBARRAYS") {
        int targetSum;
        ss >> targetSum;
        array.findSubarraysWithSum(targetSum);
    } else {
        cout << "Ошибка: неизвестная операция." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Использование: ./array --file <имя_файла> --query '<команда>'\n";
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

    Array array;
    array.loadFromJson(filename);
    processCommand(array, filename, command);
    array.saveToJson(filename);

    return 0;
}

