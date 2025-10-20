#include "../include/Crypto.h"
#include <cctype> // for toupper/tolower

std::string Crypto::encrypt(const std::string& text, int key) {
    std::string result = text;
    
    // 确保密钥在0-25范围内
    key = key % 26;
    
    for (char &c : result) {
        if (isupper(c)) {
            c = 'A' + (c - 'A' + key) % 26;
        } else if (islower(c)) {
            c = 'a' + (c - 'a' + key) % 26;
        }
        // 其他字符保持不变
    }
    
    return result;
}

std::string Crypto::decrypt(const std::string& text, int key) {
    // 解密就是加密的逆过程，使用负密钥
    return encrypt(text, 26 - (key % 26));
}
