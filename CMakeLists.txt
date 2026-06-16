cmake_minimum_required(VERSION 3.16)
project(LinuxWGGUI VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# КРИТИЧЕСКИ ВАЖНО: Включаем автогенерацию кода для UI и логики Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 

# Ищем компоненты Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

# Явно перечисляем все файлы. 
# Убедись, что на диске файл называется именно mainwindow.ui!
add_executable(LinuxWGGUI 
    main.cpp 
    mainwindow.cpp 
    mainwindow.h 
    mainwindow.ui
    settingsdialog.cpp    # <-- Маленькими буквами!
    settingsdialog.h      # <-- Маленькими буквами!
    settingsdialog.ui
)

# Линкуем библиотеки Qt
target_link_libraries(LinuxWGGUI PRIVATE Qt6::Widgets)
# --- НАСТРОЙКА СБОРКИ ПАКЕТОВ (CPACK) ---
set(CPACK_PACKAGE_NAME "linuxwggui")
set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_VENDOR "K5ilnik")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Графический интерфейс для WireGuard в Linux")
set(CPACK_PACKAGE_CONTACT "gladyrevv21@gmail.com")

# Куда устанавливать бинарник внутри пакета
install(TARGETS LinuxWGGUI DESTINATION bin)

# Настройки для DEB (Ubuntu/Debian)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "K5ilnik <gladyrevv21@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "wireguard, wireguard-tools") # Пакет сам установит зависимости!

# Настройки для RPM (Fedora/RedHat)
set(CPACK_RPM_PACKAGE_REQUIRES "wireguard-tools")

# Указываем, какие языковые файлы мы хотим создать (например, английский)

set(TS_FILES
    LinuxWGGUI_uk.ts
    LinuxWGGUI_be.ts
    LinuxWGGUI_en.ts
    LinuxWGGUI_de.ts
    LinuxWGGUI_fr.ts
    LinuxWGGUI_it.ts
)

# Команда Qt6 для автоматической сборки переводов
qt6_add_translations(LinuxWGGUI
    TS_FILES ${TS_FILES}
)

include(CPack)
