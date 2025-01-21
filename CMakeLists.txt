cmake_minimum_required(VERSION 3.30)
project(Enigma)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Sql REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR} src)

add_executable(Enigma src/main.cpp
        src/core/dbmanager.h
        src/core/dbmanager.cpp
        src/core/encryption.h
        src/core/encryption.cpp
        src/models/user.cpp
        src/models/user.h
        src/models/passwordmanager.cpp
        src/models/passwordmanager.h
        src/ui/mainwindow.cpp
        src/ui/mainwindow.h
        src/ui/loginwidget.cpp
        src/ui/loginwidget.h
        src/ui/passwordmanagerwidget.cpp
        src/ui/passwordmanagerwidget.h
        src/ui/passwordgeneratorwidget.cpp
        src/ui/passwordgeneratorwidget.h
        src/ui/logindialog.h
        src/ui/logindialog.cpp
        src/core/totpgenerator.h
        src/core/totpgenerator.cpp
        src/models/notemanager.h
        src/models/notemanager.cpp
        src/ui/notepadwidget.h
        src/ui/notepadwidget.cpp)

target_link_libraries(Enigma
        Qt5::Widgets
        Qt5::Sql
        ${OPENSSL_LIBRARIES}
        mysqlclient
)