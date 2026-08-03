cmake_minimum_required(VERSION 3.16)
project(LogLib)

# Опция для автоматического экспорта символов в DLL (актуально только для Windows)
# На Linux/macOS это не нужно, но на Windows без этого динамическая библиотека не соберется
# set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Создаем динамическую библиотеку (SHARED)
add_library(LogLib SHARED
    src/LogLib/Includes.h
    src/LogLib/Level.h          src/LogLib/Level.cpp
    src/LogLib/Log.h            src/LogLib/Log.cpp
    src/LogLib/LogLib.h         src/LogLib/LogLib.cpp
)

# Указываем директорию с заголовочными файлами.
# PUBLIC означает, что этот путь "пробросится" и в TestLogLib, 
# чтобы в Test.cpp работали инструкции #include "LogLib.h"
target_include_directories(LogLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/LogLib)

# Находим пакет для работы с потоками в системе
find_package(Threads REQUIRED)

# Создаем исполняемый файл (только Test.cpp)
add_executable(TestLogLib 
    src/TestApp/Test.cpp
)

# Подключаем библиотеку LogLib к исполняемому файлу TestLogLib
target_link_libraries(TestLogLib PRIVATE LogLib)

# Подключаем библиотеку потоков
target_link_libraries(TestLogLib PRIVATE Threads::Threads)