#include "Level.h"

using namespace std;       // Пространство имен (мне так удобней)

// Преобразование уровня в строку для записи в журнал
string levelToString(Level lvl) {
    switch (lvl) {
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR:   return "ERROR";
        default:      return "UNKNOWN";
    }
}

// Преобразование строки в уровень важности
// Бросает исключение, если строка не соответствует ни одному уровню
Level stringToLevel(const string& str) {
    if (str == "INFO")    return Level::INFO;
    if (str == "WARNING") return Level::WARNING;
    if (str == "ERROR" || str == "Eror")   return Level::ERROR;
    return Level::UNKNOWN;
}