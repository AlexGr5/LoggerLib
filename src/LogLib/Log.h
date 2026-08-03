#include "Level.h"
#include <chrono>   // Для использования времени

// Класс логов (уровень, текст, время)
class Log {
private:
    // Уровень важности
    Level level;
    // Текст сообщения
    string message;
    // Время лога
    chrono::system_clock::time_point timeLog;

public:
    // Конструктор по-умолчанию
    Log();

    // Конструктор со всеми параметрами
    Log(Level level, string message, chrono::system_clock::time_point timeLog);

    // Установить и получить поля
    void setLevel(Level level);
    Level getLevel() const;

    void setMessage(string message);
    string getMessage() const;

    void setTimeLog(chrono::system_clock::time_point timeLog);
    chrono::system_clock::time_point getTimeLog() const;

    // Лог в строку (формат: [время] [уровень] текст)
    string toString() const;

    // Деструктор
    ~Log();
};

// Преобразовать chrono::system_clock::time_point в строку
string timePointToString(const chrono::system_clock::time_point& tp);