#include "Log.h"
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;       // Пространство имен (мне так удобней)

// Конструктор по-умолчанию
Log::Log() : level(Level::INFO), message{}, timeLog(chrono::system_clock::now()) {}

// Конструктор со всеми параметрами
Log::Log(Level level, const string& message, chrono::system_clock::time_point timeLog)
    : level(level), message(message), timeLog(timeLog) {}

// Установить и получить поля
void Log::setLevel(Level level) { this->level = level; }
Level Log::getLevel() const { return level; }

void Log::setMessage(const string& message) { this->message = message; }
string Log::getMessage() const { return message; }

void Log::setTimeLog(chrono::system_clock::time_point timeLog) { this->timeLog = timeLog; }
chrono::system_clock::time_point Log::getTimeLog() const { return timeLog; }

// Лог в строку: "[2025-01-15 14:30:45] [WARNING] Текст сообщения"
string Log::toString() const {
    return "[" + timePointToString(timeLog) + "] [" + levelToString(level) + "] " + message;
}

// Преобразовать chrono::system_clock::time_point в строку формата "ГГГГ-ММ-ДД ЧЧ:ММ:СС"
string timePointToString(const chrono::system_clock::time_point& tp) {
    time_t time = chrono::system_clock::to_time_t(tp);
    tm tm_buf;
    localtime_r(&time, &tm_buf);  // Потокобезопасная версия localtime

    ostringstream oss;
    oss << put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}