#include "LogLib.h"
#include <iostream>

using namespace std;       // Пространство имен (мне так удобней)

// ============================================================
// Конструкторы
// ============================================================

// Конструктор с именем файла (уровень по умолчанию — INFO)
LogLib::LogLib(const string& fileName)
    : fileName((fileName)), minLevel(Level::INFO), isRunning(true)
{
    // Запускаем фоновый поток-потребитель сразу при создании объекта
    workerThread = thread(&LogLib::workerFunction, this);
}

// Конструктор с именем файла и минимальным уровнем важности
LogLib::LogLib(const string& fileName, Level minLevel)
    : fileName((fileName)), minLevel(minLevel), isRunning(true)
{
    // Запускаем фоновый поток-потребитель сразу при создании объекта
    workerThread = thread(&LogLib::workerFunction, this);
}

// ============================================================
// Геттеры / Сеттеры
// ============================================================

// Установить минимальный уровень
void LogLib::setMinLevel(Level newMinLevel) {
    lock_guard<mutex> lock(queueMutex);
    this->minLevel = newMinLevel;
}

// Получить минимальный уровень
Level LogLib::getMinLevel() const {
    return minLevel;
}

// Получить имя файла
string LogLib::getFileName() const {
    return fileName;
}

// ============================================================
// Методы добавления логов (Producer — Производитель)
// Каждый метод мгновенно кладёт лог в очередь и возвращает управление.
// Запись на диск происходит асинхронно в фоновом потоке.
// ============================================================

// Добавить лог с сообщением (уровень по умолчанию)
void LogLib::addLog(const string& message) {
    addLog(minLevel, (message), chrono::system_clock::now());
}

// Добавить лог с сообщением и уровнем важности
void LogLib::addLog(Level level, const string& message) {
    addLog(level, (message), chrono::system_clock::now());
}

// Добавить лог с сообщением и временем
void LogLib::addLog(const string& message, chrono::system_clock::time_point timeLog) {
    addLog(minLevel, (message), timeLog);
}

// Добавить лог в очередь со всеми параметрами.
// Это основной метод Producer: захватывает мьютекс, кладёт лог в очередь,
// будит Consumer и мгновенно возвращается.
void LogLib::addLog(Level level, const string& message, chrono::system_clock::time_point timeLog) {
    // Фильтрация: сообщения с уровнем НИЖЕ минимального не записываются
    if (level < minLevel) {
        return;
    }

    // Формируем объект лога
    Log logEntry(level, (message), timeLog);

    {
        // Критическая секция: защищаем доступ к очереди
        lock_guard<mutex> lock(queueMutex);
        logQueue.push((logEntry));
    }

    // Уведомляем фоновый поток (Consumer), что в очереди появился новый лог.
    // Если Consumer спит на cv.wait() — он проснётся и запишет лог.
    cv.notify_one();
}

// ============================================================
// Фоновый поток-потребитель (Consumer)
// ============================================================

void LogLib::workerFunction() {
    // Открываем файл один раз при старте потока (режим дозаписи).
    // Это эффективнее, чем открывать/закрывать файл на каждую запись.
    ofstream outFile(fileName, ios::app);

    if (!outFile.is_open()) {
        // Обработка ошибки: не удалось открыть файл для записи
        cerr << "[LogLib] Ошибка: не удалось открыть файл \""
                  << fileName << "\" для записи логов!" << endl;
        return;
    }

    while (true) {
        unique_lock<mutex> lock(queueMutex);

        // Consumer засыпает, пока очередь пуста и поток не остановлен.
        // cv.wait атомарно отпускает мьютекс и засыпает,
        // а при пробуждении — снова захватывает мьютекс.
        cv.wait(lock, [this]() {
            return !logQueue.empty() || !isRunning;
        });

        // Если поток останавливается и очередь пуста — выходим из цикла
        if (!isRunning && logQueue.empty()) {
            break;
        }

        // Записываем все накопившиеся логи из очереди.
        // Это позволяет обработать пачку за одну итерацию,
        // уменьшая количество переключений контекста.
        while (!logQueue.empty()) {
            // Забираем лог из головы очереди
            Log currentLog = (logQueue.front());
            logQueue.pop();

            // Разблокируем мьютекс на время записи в файл,
            // чтобы Producer (addLog) не ждал медленную операцию I/O
            lock.unlock();

            outFile << currentLog.toString() << "\n";
            outFile.flush();  // Гарантируем, что данные ушли на диск

            // Снова захватываем мьютекс для доступа к очереди
            lock.lock();
        }
    }

    // Поток завершается: закрываем файл
    outFile.close();
}

// ============================================================
// Деструктор
// ============================================================

// Деструктор: корректно останавливает фоновый поыток.
// Гарантирует, что все логи из очереди будут записаны на диск
// перед уничтожением объекта.
LogLib::~LogLib() {
    // Выставляем флаг остановки
    isRunning = false;

    // Будим Consumer, чтобы он увидел isRunning == false и вышел из wait
    cv.notify_one();

    // Ждём, пока фоновый поток допишет оставшиеся логи и завершится
    if (workerThread.joinable()) {
        workerThread.join();
    }
}
