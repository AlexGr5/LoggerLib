#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "LogLib.h"

// Основной цикл пользовательского интерфейса
void loop();

// Проверка ввода "1" или "2"
bool oneOrTwo(const string& str);

// Тест работы библиотеки без пользователя (автоматический)
void afkTest();

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    afkTest();
    loop();
    return 0;
}

// ============================================================
// Основной цикл пользовательского интерфейса
// ============================================================
void loop() {
    bool flagExit = true;               // Флаг выхода из основного цикла
    bool flagInputNewFile = true;       // Флаг выхода из ввода нового файла
    bool flagRetryInputNewFile = true;  // Флаг повторного ввода файла
    bool flagCorrectNumber = false;     // Флаг верно выбранного файла из уже введенных
    string inputFileName = "";          // Ввод имени файла
    string inputLevel = "";             // Ввод минимального уровня важности
    string inputStr = "";               // Для ввода различных данных

    // Список разных файлов (каждый LogLib содержит свой фоновый поток-Consumer)
    vector<unique_ptr<LogLib>> logFiles;

    LogLib* oneLogToWork = nullptr;     // Указатель на текущий файл для работы с ним

    // Приветствующая шапка
    cout << "========================================" << endl;
    cout << "\tСистема ручного логирования." << endl;
    cout << "Введите имя файла для записи логов и" << endl;
    cout << "уровень важности по умолчанию." << endl;
    cout << "\t\tНапример:" << endl;
    cout << "FileName.txt и ERROR/WARNING/INFO" << endl;
    cout << "========================================" << endl;

    // Основной цикл
    while (flagExit) {
        // Проверка для повторного ввода новых файлов
        if (logFiles.size() > 0) {
            cout << "\n-----------------------------------" << endl;
            cout << "Доступные действия:" << endl;
            cout << "1. Работать с существующими файлами." << endl;
            cout << "2. Добавить новый файл." << endl;
            cout << "Ввод: ";
            getline(cin, inputStr);
            // Проверка что выбрали
            flagRetryInputNewFile = oneOrTwo(inputStr);
        }

        // Ввод нового файла
        if (flagRetryInputNewFile) {
            do {
                cout << "\nВведите названия файла для логов( например FileName.txt ): ";
                getline(cin, inputFileName);
                cout << endl;
                cout << "\nВведите уровень важности( один из: ERROR/WARNING/INFO ): ";
                getline(cin, inputLevel);
                cout << endl;

                // Создаём новый LogLib — внутри сразу стартует фоновый поток записи
                try {
                    Level lvl = stringToLevel(inputLevel);
                    logFiles.push_back(make_unique<LogLib>(inputFileName, lvl));
                } catch (...) {
                    logFiles.push_back(make_unique<LogLib>(inputFileName, INFO));
                }

                inputStr = "";
                cout << "\n-----------------------------------" << endl;
                cout << "Вы добавили файл для записи логов." << endl;
                cout << "Доступные действия:" << endl;
                cout << "1. Работать с файлами." << endl;
                cout << "2. Добавить ещё файл." << endl;
                cout << "Ввод: ";
                getline(cin, inputStr);
                // Проверка что выбрали
                flagInputNewFile = oneOrTwo(inputStr);
            } while (flagInputNewFile);
        }

        bool pressQ = false;

        // Выбор файла для работы с ним
        do {
            cout << "\n-----------------------------------" << endl;
            cout << "Доступные файлы для работы:" << endl;
            int i = 1;
            for (size_t i = 0; i < logFiles.size(); ++i) {
                cout << i + 1 << ". " << logFiles[i]->getFileName() << endl;
            }
            cout << endl;
            inputStr = "";
            cout << "Выберите тот, с которым хотите работать сейчас." << endl;
            cout << "Введите номер файла без точки (q - выход): ";
            getline(cin, inputStr);

            // Обработка ввода
            flagCorrectNumber = false;
            if ((inputStr != "Q") && (inputStr != "q")) {
                pressQ = true;
                int choiseNumber = 0;
                try {
                    choiseNumber = stoi(inputStr);
                } catch (...) {
                    cerr << "Ошибка: введите число." << endl;
                    flagCorrectNumber = true;
                    continue;
                }
                flagCorrectNumber = true;
                oneLogToWork = nullptr;
                // Проверяем, что номер верный
                if (choiseNumber >= 1 && choiseNumber <= (int)logFiles.size()) {
                    // Получаем указатель
                    oneLogToWork = logFiles[choiseNumber - 1].get();
                    flagCorrectNumber = false;
                }
            } else {
                // Пользователь нажал 'q' — выходим
                flagExit = false;
                break;
            }
        } while (flagCorrectNumber);

        // Ввод логов
        if ((oneLogToWork != nullptr) && (pressQ)) {
            bool flagInputLogs = true;
            do {
                string inputMessage = "";
                cout << "\n************************" << endl;
                cout << "Введите текст ЛОГА: ";
                getline(cin, inputMessage);
                cout << endl;

                inputStr = "";
                cout << "Введите уровень важности ЛОГА (ERROR/WARNING/INFO)" << endl;
                cout << "или пустой символ, если хотите использовать" << endl;
                cout << "уровень важности по-умолчанию." << endl;
                cout << "Ввод: ";
                getline(cin, inputStr);
                cout << endl;

                // Проверка на пустую строку
                Level lvl;
                if (inputStr.empty()) {
                    lvl = oneLogToWork->getMinLevel();
                } else {
                    try {
                        lvl = stringToLevel(inputStr);
                    } catch (const exception& e) {
                        cerr << "Неверный уровень, используется уровень по умолчанию." << endl;
                        lvl = oneLogToWork->getMinLevel();
                    }
                }

                cout << "\nВремя лога взято текущее!" << endl;

                // addLog() мгновенно кладёт лог в очередь (Producer)
                // и возвращает управление. Фоновый поток (Consumer)
                // сам заберёт лог и запишет в файл.
                // Главный поток не блокируется и сразу готов принять новый ввод.
                oneLogToWork->addLog(lvl, inputMessage);

                cout << "[Система] Сообщение передано в очередь на запись." << endl;

                inputStr = "";
                cout << "\n--------------" << endl;
                cout << "Ввести ещё лог?" << endl;
                cout << "1. Нет" << endl;
                cout << "2. Да" << endl;
                cout << "Ввод: ";
                getline(cin, inputStr);
                // Проверка что выбрали
                flagInputLogs = oneOrTwo(inputStr);
            } while (flagInputLogs);
        }

        // Итоговые действия
        bool flagInputItog = false;
        do {
            inputStr = "";
            cout << "\n---------------" << endl;
            cout << "Выбор действий:" << endl;
            cout << "1. Завершить работу" << endl;
            cout << "2. Продолжить работу" << endl;
            cout << "Ввод: ";
            getline(cin, inputStr);

            int inputNumber = 0;
            try {
                inputNumber = stoi(inputStr);
            } catch (...) {
                cerr << "Ошибка ввода!" << endl;
                flagInputItog = true;
                continue;
            }

            switch (inputNumber) {
                // Выход
                case 1: {
                    flagExit = false;
                    break;
                }
                // Повторная работа
                case 2: {
                    flagExit = true;
                    break;
                }
                // Повтор ввода
                default: {
                    cout << "Ошибка ввода!" << endl;
                    flagInputItog = true;
                    break;
                }
            }
        } while (flagInputItog);
    }

    // При выходе из функции вектор logFiles уничтожается.
    // Деструктор каждого LogLib:
    //   1. Выставляет isRunning = false
    //   2. Будит фоновый поток
    //   3. join() — ждёт, пока все логи из очереди запишутся на диск
    cout << "\nЗавершение работы. Ожидание записи всех логов на диск..." << endl;
}

// ============================================================
// Проверить строку на 1 или 2 для принятия решений
// 1 - false, 2 - true
// ============================================================
bool oneOrTwo(const string& str) {
    try {
        int number = stoi(str);
        if (number == 1) return false;
        if (number == 2) return true;
    } catch (...) {
        cerr << "Ошибка: введите 1 или 2." << endl;
    }
    return false;  // По умолчанию — "нет"
}

// ============================================================
// Тест работы библиотеки без пользователя (автоматический)
// Демонстрирует Producer-Consumer: логи добавляются из main-потока,
// а фоновые потоки внутри каждого LogLib записывают их на диск.
// ============================================================
void afkTest() {
    auto myLogs = make_unique<LogLib>("name1.txt", WARNING);
    auto noMyLogs = make_unique<LogLib>("name2.txt", INFO);

    // Producer: кладём логи в очереди
    myLogs->addLog(ERROR, "1 Everything is bad!");
    myLogs->addLog(WARNING, "1 Could be better.");
    myLogs->addLog(INFO, "1 Everything is fine!");  // Не запишется (INFO < WARNING)

    noMyLogs->addLog(ERROR, "2 Everything is bad!");
    noMyLogs->addLog(WARNING, "2 Could be better.");
    noMyLogs->addLog(INFO, "2 Everything is fine!");

    cout << "Логи добавлены в очереди. Запись на диск идёт в фоновых потоках." << endl;

    // При выходе из функции деструкторы myLogs и noMyLogs
    // остановят фоновые потоки и дождутся записи всех логов.
}