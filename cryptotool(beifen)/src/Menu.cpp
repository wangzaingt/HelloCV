#include "../include/Menu.h"
#include <iostream>
#include "../include/Crypto.h"
#include "../include/FileHandler.h"

void Menu::showMenu() {
    int choice;
    std::string inputText, encryptedText, decryptedText;
    int key;
    std::string inputFilePath, outputFilePath;
    
    while (true) {
        std::cout << "\n=== 文本加密解密工具 ===\n";
        std::cout << "1. 文本加密\n";
        std::cout << "2. 文本解密\n";
        std::cout << "3. 文件加密\n";
        std::cout << "4. 文件解密\n";
        std::cout << "0. 退出\n";
        std::cout << "请选择功能: ";
        
        std::cin >> choice;
        std::cin.ignore(); // 清除输入缓冲区
        
        switch (choice) {
            case 1: // 文本加密
                std::cout << "请输入要加密的文本: ";
                std::getline(std::cin, inputText);
                
                std::cout << "请输入密钥(整数): ";
                std::cin >> key;
                std::cin.ignore();
                
                encryptedText = Crypto::encrypt(inputText, key);
                std::cout << "加密结果: " << encryptedText << std::endl;
                break;
                
            case 2: // 文本解密
                std::cout << "请输入要解密的文本: ";
                std::getline(std::cin, encryptedText);
                
                std::cout << "请输入密钥(整数): ";
                std::cin >> key;
                std::cin.ignore();
                
                decryptedText = Crypto::decrypt(encryptedText, key);
                std::cout << "解密结果: " << decryptedText << std::endl;
                break;
                
            case 3: // 文件加密
                std::cout << "请输入要加密的文件路径: ";
                std::getline(std::cin, inputFilePath);
                
                std::cout << "请输入密钥(整数): ";
                std::cin >> key;
                std::cin.ignore();
                
                inputText = FileHandler::readFile(inputFilePath);
                if (inputText.empty()) {
                    std::cout << "无法读取文件或文件为空!\n";
                    break;
                }
                
                encryptedText = Crypto::encrypt(inputText, key);
                outputFilePath = "encrypted_" + inputFilePath;
                if (FileHandler::writeFile(outputFilePath, encryptedText)) {
                    std::cout << "文件加密成功! 结果已保存到: " << outputFilePath << std::endl;
                } else {
                    std::cout << "文件写入失败!\n";
                }
                break;
                
            case 4: // 文件解密
                std::cout << "请输入要解密的文件路径: ";
                std::getline(std::cin, inputFilePath);
                
                std::cout << "请输入密钥(整数): ";
                std::cin >> key;
                std::cin.ignore();
                
                encryptedText = FileHandler::readFile(inputFilePath);
                if (encryptedText.empty()) {
                    std::cout << "无法读取文件或文件为空!\n";
                    break;
                }
                
                decryptedText = Crypto::decrypt(encryptedText, key);
                outputFilePath = "decrypted_" + inputFilePath;
                if (FileHandler::writeFile(outputFilePath, decryptedText)) {
                    std::cout << "文件解密成功! 结果已保存到: " << outputFilePath << std::endl;
                } else {
                    std::cout << "文件写入失败!\n";
                }
                break;
                
            case 0: // 退出
                std::cout << "感谢使用，再见!\n";
                return;
                
            default:
                std::cout << "无效的选择，请重新输入!\n";
        }
    }
}
