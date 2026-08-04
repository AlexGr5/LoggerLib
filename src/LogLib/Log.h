#pragma once

#include "Level.h"
#include <chrono>   // Для использования времени

// Класс логов (уровень, текст, время)
class Log {
private:
    // Уровень важности
    Level level;
    // Текст сообщения
    std::string message;
    // Время лога
    std::chrono::system_clock::time_point timeLog;

public:
    // Конструктор по-умолчанию
    Log();

    // Конструктор со всеми параметрами
    Log(Level level, const std::string& message, std::chrono::system_clock::time_point timeLog);

    // Установить и получить поля
    void setLevel(Level level);
    Level getLevel() const;

    void setMessage(const std::string& message);
    std::string getMessage() const;

    void setTimeLog(std::chrono::system_clock::time_point timeLog);
    std::chrono::system_clock::time_point getTimeLog() const;

    // Лог в строку (формат: [время] [уровень] текст)
    std::string toString() const;
};

// Преобразовать chrono::system_clock::time_point в строку
std::string timePointToString(const std::chrono::system_clock::time_point& tp);
