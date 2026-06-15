cmake_minimum_required(VERSION 3.16)
project(LinuxWGGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# КРИТИЧЕСКИ ВАЖНО: Включаем автогенерацию кода для UI и логики Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 

# Ищем компоненты Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Явно перечисляем все файлы. 
# Убедись, что на диске файл называется именно mainwindow.ui!
add_executable(LinuxWGGUI 
    main.cpp 
    mainwindow.cpp 
    mainwindow.h 
    mainwindow.ui
)

# Линкуем библиотеки Qt
target_link_libraries(LinuxWGGUI PRIVATE Qt6::Widgets)