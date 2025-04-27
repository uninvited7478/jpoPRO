cmake_minimum_required(VERSION 3.16)

project(jpoPRO VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    Test
    Network
    Charts
    Location
    Positioning
    QuickControls2
)

qt_standard_project_setup(REQUIRES 6.8)

# Główna aplikacja
qt_add_executable(appjpoPRO
    main.cpp
    api_client.h api_client.cpp
    data_manager.h data_manager.cpp
    network_checker.h network_checker.cpp
    network_status.h network_status.cpp
)

qt_add_qml_module(appjpoPRO
    URI jpoPRO
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES
        api_client.h api_client.cpp
        data_manager.h data_manager.cpp
        network_checker.h network_checker.cpp
        network_status.h network_status.cpp
)

set_target_properties(appjpoPRO PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appjpoPRO
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Test
    Qt6::Quick
    Qt6::Network
    Qt6::Charts
    Qt6::Location
    Qt6::Positioning
    Qt6::QuickControls2
)

# Testy jednostkowe
enable_testing()

# Test dla DataManager
qt_add_executable(testDataManager
    tests/test_data_manager.cpp
    data_manager.h data_manager.cpp
)

target_include_directories(testDataManager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(testDataManager
    PRIVATE
    Qt6::Core
    Qt6::Test
)

add_test(NAME DataManagerTest COMMAND testDataManager)

include(GNUInstallDirs)
install(TARGETS appjpoPRO
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
