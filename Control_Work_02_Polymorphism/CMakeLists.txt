cmake_minimum_required(VERSION 3.16)
project(Control_Work_02_Polymorphism LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

add_executable(Control_Work_02_Polymorphism
    main.cpp
    uav.h uav.cpp
    sensor.h sensor.cpp
    navigation.h navigation.cpp
    controller.h controller.cpp
    autopilot.h autopilot.cpp
    mainwindow.h mainwindow.cpp
)

target_link_libraries(Control_Work_02_Polymorphism PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
