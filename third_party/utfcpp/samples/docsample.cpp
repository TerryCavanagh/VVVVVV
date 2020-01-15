#include "../source/utf8.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "\nUsage: docsample filename\n";
        return 0;
    }
    const char* test_file_path = argv[1];
    // Open the test file (must be UTF-8 encoded)
    ifstream fs8(test_file_path);
    if (!fs8.is_open()) {
        cout << "Could not open " << test_file_path << endl;
        return 0;
    }

    unsigned line_count = 1;
    string line;
    // Play with all the lines in the file
    while (getline(fs8, line)) {
        // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
#if __cplusplus >= 201103L // C++ 11 or later
        auto end_it = utf8::find_invalid(line.begin(), line.end());
#else
        string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
#endif // C++ 11
        if (end_it != line.end()) {
            cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
            cout << "This part is fine: " << string(line.begin(), end_it) << "\n";
        }
        // Get the line length (at least for the valid part)
        ptrdiff_t length = utf8::distance(line.begin(), end_it);
        cout << "Length of line " << line_count << " is " << length <<  "\n";

        // Convert it to utf-16
#if __cplusplus >= 201103L // C++ 11 or later
        u16string utf16line = utf8::utf8to16(line);
#else
        vector<unsigned short> utf16line;
        utf8::utf8to16(line.begin(), end_it, back_inserter(utf16line));
#endif // C++ 11
        // And back to utf-8;
#if __cplusplus >= 201103L // C++ 11 or later
        string utf8line = utf8::utf16to8(utf16line);
#else
        string utf8line; 
        utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(utf8line));
#endif // C++ 11
        // Confirm that the conversion went OK:
        if (utf8line != string(line.begin(), end_it))
            cout << "Error in UTF-16 conversion at line: " << line_count << "\n";        

        line_count++;
    } 

    return 0;
}
