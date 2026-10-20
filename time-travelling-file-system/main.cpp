#include <iostream>
#include <string>
#include "FileSystem.hpp"

// Reads one command per line from stdin until EXIT or end of input.
int main() {
    FileSystem fs;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();  // Windows line endings
        if (!fs.execute(line)) break;
    }
    return 0;
}
