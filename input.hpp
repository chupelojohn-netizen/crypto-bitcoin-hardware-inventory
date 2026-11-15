#pragma once

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <string>

namespace cryptid {
using namespace std;

// -------------------- Input helpers --------------------

inline string lowerCopy(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
    return value;
}

inline int readInt(const string& prompt) {
    int value = 0;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof()) {
            return 0;
        }
        cout << "Please enter a whole number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

inline double readDouble(const string& prompt) {
    double value = 0.0;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof()) {
            return 0.0;
        }
        cout << "Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

inline string readLine(const string& prompt) {
    cout << prompt;
    string value;
    if (!getline(cin, value)) {
        return "";
    }
    return value;
}

inline char readChar(const string& prompt) {
    while (true) {
        string value = readLine(prompt);
        if (value.empty()) {
            if (cin.eof()) {
                return 'q';
            }
            continue;
        }
        return static_cast<char>(tolower(static_cast<unsigned char>(value[0])));
    }
}

} // namespace cryptid
