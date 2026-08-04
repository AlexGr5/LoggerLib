#pragma once

#include "Log.h"

#include <queue>                // Очередь
#include <mutex>                // Мьютекс
#include <condition_variable>   // Условная переменая
#include <thread>               // Потоки
#include <atomic>               // Атомарные действия
#include <fstream>              // Запись в файл

// Класс для работы с логами.
// Реализует паттерн "Производитель-Потребитель" (Producer-Consumer):
//   - addLog() — Производитель: кладёт лог в очередь и мгновенно возвращает управление.
//   - workerFunction() — Потребитель: фоновый поток, забирает логи из очереди
//     и записывает их в файл на диске.
//
// Это гарантирует, что главный поток приложения не блокируется при записи на диск,
// а передача данных между потоками потокобезопасна.
class LogLib {
private:
    // Название файла для записей логов
    std::string fileName;

    // Минимальный уровень важности записи логов.
    // Сообщения с уровнем ниже заданного не записываются в журнал.
    Level minLevel;

    // --- Producer-Consumer ---
    // Очередь логов, Producer кладёт сюда, Consumer забирает отсюда
    std::queue<Log> logQueue;

    // Мьютекс для защиты очереди logQueue и поля minLevel
    std::mutex queueMutex;

    // Условная переменная, Consumer спит, пока очередь пуста
    std::condition_variable cv;

    // Фоновый поток-потребитель (Consumer), который пишет логи в файл
    std::thread workerThread;

    // Флаг пока true — поток работает; при false — поток завершается
    std::atomic<bool> isRunning;

    // Функция фонового потока (Потребитель).
    // Ждёт появления логов в очереди и записывает их в файл.
    void workerFunction();

public:
    // Конструктор с именем файла (уровень по умолчанию — INFO)
    LogLib(const std::string& fileName);

    // Конструктор с именем файла и минимальным уровнем важности
    LogLib(const std::string& fileName, Level minLevel);

    // Запрещаем копирование, так как класс владеет потоком и мьютексом
    LogLib(const LogLib&) = delete;
    LogLib& operator=(const LogLib&) = delete;

    // Установить минимальный уровень
    void setMinLevel(Level newMinLevel);

    // Получить минимальный уровень
    Level getMinLevel() const;

    // Получить имя файла
    std::string getFileName() const;

    // Добавить лог с сообщением (уровень = minLevel по умолчанию)
    void addLog(const std::string& message);

    // Добавить лог с сообщением и уровнем важности
    void addLog(Level level, const std::string& message);

    // Добавить лог с сообщением и временем
    void addLog(const std::string& message, std::chrono::system_clock::time_point timeLog);

    // Добавить лог в очередь со всеми параметрами
    void addLog(Level level, const std::string& message, std::chrono::system_clock::time_point timeLog);

    // Деструктор: останавливает фоновый поток, дожидается записи всех логов
    ~LogLib();
};