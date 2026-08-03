#include "Level.h"

// Преобразование уровня в строку для записи в журнал
string levelToString(Level lvl) {
    switch (lvl) {
        case INFO:    return "INFO";
        case WARNING: return "WARNING";
        case ERROR:   return "ERROR";
        default:      return "UNKNOWN";
    }
}

// Преобразование строки в уровень важности
// Бросает исключение, если строка не соответствует ни одному уровню
Level stringToLevel(const string& str) {
    if (str == "INFO")    return INFO;
    if (str == "WARNING") return WARNING;
    if (str == "ERROR")   return ERROR;
    throw invalid_argument("Неверный уровень важности: " + str);
}