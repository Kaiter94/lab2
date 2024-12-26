#include <iostream>
#include <string>
#include <cstring>
using namespace std;

class Stack {
private:
    string data[1000];
    int top;          

public:
    Stack() : top(-1) {}

    bool isEmpty() const {
        return top == -1;
    }

    bool isFull() const {
        return top == 999;
    }

    void push(const string& element) {
        if (isFull()) {
            cout << "Ошибка: Стек переполнен!" << endl;
            return;
        }
        data[++top] = element;
    }

    string pop() {
        if (isEmpty()) {
            cout << "Ошибка: Стек пуст!" << endl;
            return "";
        }
        return data[top--];
    }

    string peek() const {
        if (isEmpty()) {
            cout << "Ошибка: Стек пуст!" << endl;
            return "";
        }
        return data[top];
    }
};

// Проверка является ли символ тегом
bool isOpeningTag(const string& tag) {
    return tag.front() == '<' && tag.back() == '>' && tag[1] != '/';
}

bool isClosingTag(const string& tag) {
    return tag.front() == '<' && tag.back() == '>' && tag[1] == '/';
}

// Извлечение имени тега из строки
string extractTagName(const string& tag) {
    if (isOpeningTag(tag)) {
        return tag.substr(1, tag.size() - 2);
    }
    if (isClosingTag(tag)) {
        return tag.substr(2, tag.size() - 3);
    }
    return "";
}

bool isValidXML(const string& xml) {
    Stack stack;
    size_t i = 0;

    while (i < xml.size()) {
        if (xml[i] == '<') {
            size_t j = i;
            while (j < xml.size() && xml[j] != '>') ++j;

            if (j == xml.size()) return false;

            string tag = xml.substr(i, j - i + 1);

            if (isOpeningTag(tag)) {
                stack.push(extractTagName(tag));
            } else if (isClosingTag(tag)) {
                if (stack.isEmpty() || stack.peek() != extractTagName(tag)) {
                    return false;Несоответствие тегов
                }
                stack.pop();
            }

            i = j;
        }
        ++i;
    }

    return stack.isEmpty();
}


string fixXML(string xml) {
    for (size_t i = 0; i < xml.size(); ++i) {
        char original = xml[i];

        for (char c : string("<>/abcdefghijklmnopqrstuvwxyz")) {
            if (c == original) continue;

            xml[i] = c;

            if (isValidXML(xml)) {
                return xml;
            }
        }

        xml[i] = original;
    }

    return "Ошибка: Невозможно восстановить XML";
}

int main() {
    string xml;
    cout << "Введите повреждённую XML строку: ";
    getline(cin, xml);

    string result = fixXML(xml);
    cout << "Восстановленная XML строка: " << result << endl;

    return 0;
}

