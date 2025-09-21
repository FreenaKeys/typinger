#include "fileapi.h"
#include <iostream>
#include <fstream>

namespace FileAPI {

int overwriteFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if(file.is_open()) {
        file << content;
        file.close();
        return 0; // Success
    } else {
        return -1; // Error opening file
    }
}

int writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::app); // Append mode
    if(file.is_open()) {
        file << content;
        file.close();
        return 0; // Success
    } else {
        return -1; // Error opening file
    }
}

} // namespace FileAPI