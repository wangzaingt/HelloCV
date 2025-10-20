#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>

class FileHandler {
public:
    // 读取文件内容
    static std::string readFile(const std::string& filePath);
    
    // 写入内容到文件
    static bool writeFile(const std::string& filePath, const std::string& content);
};

#endif
