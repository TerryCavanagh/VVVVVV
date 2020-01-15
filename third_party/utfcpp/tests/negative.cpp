#include "utf8.h"
using namespace utf8;

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

const unsigned INVALID_LINES[] = { 75, 76, 83, 84, 85, 93, 102, 103, 105, 106, 107, 108, 109, 110, 114, 115, 116, 117, 124, 125, 130, 135, 140, 145, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 169, 175, 176, 177, 207, 208, 209, 210, 211, 220, 221, 222, 223, 224, 232, 233, 234, 235, 236, 247, 248, 249, 250, 251, 252, 253, 257, 258, 259, 260, 261, 262, 263, 264};
const unsigned* INVALID_LINES_END = INVALID_LINES + sizeof(INVALID_LINES)/sizeof(unsigned);

int main(int argc, char** argv)
{
    string test_file_path;
    if (argc == 2) 
        test_file_path = argv[1];
    else {
        cout << "Wrong number of arguments" << endl;
        return 1;
    }
    // Open the test file
    ifstream fs8(test_file_path.c_str());
    if (!fs8.is_open()) {
        cout << "Could not open " << test_file_path << endl;
        return 1;
    }

    // Read it line by line
    unsigned int line_count = 0;
    char byte;
    while (!fs8.eof()) {
        string line;
        while ((byte = static_cast<char>(fs8.get())) != '\n' && !fs8.eof())
            line.push_back(byte);

        line_count++;
        bool expected_valid = (find(INVALID_LINES, INVALID_LINES_END, line_count) == INVALID_LINES_END);
        // Print out lines that contain unexpected invalid UTF-8
        if (!is_valid(line.begin(), line.end())) {
            if (expected_valid) {
                cout << "Unexpected invalid utf-8 at line " << line_count << '\n';
                return 1;
            }

            // try fixing it:
            string fixed_line;
            replace_invalid(line.begin(), line.end(), back_inserter(fixed_line));
            if (!is_valid(fixed_line.begin(), fixed_line.end())) {
                cout << "replace_invalid() resulted in an invalid utf-8 at line " << line_count << '\n';
                return 1;
            }
        }
        else if (!expected_valid) {
            cout << "Invalid utf-8 NOT detected at line " << line_count << '\n';
            return 1;
        }
    }
}
