#pragma once
#include <string>
namespace FileAPI
{
    int overwriteFile(const std::string& filename, const std::string& content);
    int writeFile(const std::string& filename, const std::string& content);

} // namespace FileAPI