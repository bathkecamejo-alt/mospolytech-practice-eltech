#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Простейшая хэш-функция (алгоритм DJB2) для демонстрации хэширования "с нуля"
std::string calculateHash(const std::string& str) {
    unsigned long hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return std::to_string(hash);
}

// Функция шифрования/дешифрования файла методом XOR с динамической солью
bool processFile(const std::string& inputPath, const std::string& outputPath, const std::string& key) {
    std::ifstream inFile(inputPath, std::ios::binary);
    std::ofstream outFile(outputPath, std::ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        return false;
    }

    // Хэшируем ключ, чтобы получить динамический сдвиг для гаммирования
    std::string dynamicKey = calculateHash(key);
    size_t keyIndex = 0;

    char byte;
    while (inFile.get(byte)) {
        // Побитовый XOR текущего байта с символом динамического ключа
        char processedByte = byte ^ dynamicKey[keyIndex % dynamicKey.length()];
        outFile.put(processedByte);
        keyIndex++;
    }

    inFile.close();
    outFile.close();
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "====================================================\n";
        std::cout << "Eltech Security Utility v1.0 (Crypto from Scratch)\n";
        std::cout << "====================================================\n";
        std::cout << "Использование:\n";
        std::cout << "  " << argv[0] << " <режим> <входной_файл> <выходной_файл> <ключ_или_соль>\n\n";
        std::cout << "Режимы:\n";
        std::cout << "  --encrypt : Зашифровать файл с использованием ключа\n";
        std::cout << "  --decrypt : Расшифровать файл с использованием ключа\n";
        std::cout << "  --hash    : Сгенерировать хэш строки с солью\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];
    std::string key = argv[4];

    if (mode == "--encrypt" || mode == "--decrypt") {
        std::cout << "[*] Обработка файла: " << input << " -> " << output << "...\n";
        if (processFile(input, output, key)) {
            std::cout << "[+] Операция успешно завершена.\n";
        } else {
            std::cerr << "[-] Ошибка: Не удалось открыть файлы.\n";
            return 1;
        }
    } else if (mode == "--hash") {
        // Режим хэширования строки (например данных студента) вместе с солью
        std::string saltedData = input + key;
        std::string resultHash = calculateHash(saltedData);
        
        std::ofstream outFile(output);
        if (outFile.is_open()) {
            outFile << resultHash;
            outFile.close();
            std::cout << "[+] Хэш успешно записан в файл: " << output << "\n";
            std::cout << "[*] Значение хэша: " << resultHash << "\n";
        } else {
            std::cerr << "[-] Ошибка записи хэша.\n";
            return 1;
        }
    } else {
        std::cerr << "[-] Неизвестный режим работы.\n";
        return 1;
    }

    return 0;
}
