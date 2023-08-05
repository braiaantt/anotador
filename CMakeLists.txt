cmake_minimum_required(VERSION 3.5)

project(practica_SQLite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        mydialog.cpp
        mydialog.h
        mydialog.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(practica_SQLite
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(practica_SQLite SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_library(practica_SQLite SHARED  # Agrega la opción "SHARED" aquí solo si no es Android
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(practica_SQLite PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(practica_SQLite PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS practica_SQLite
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(practica_SQLite)
endif()


find_package(Qt6 REQUIRED COMPONENTS Sql)
target_link_libraries(practica_SQLite PRIVATE Qt6::Sql)
