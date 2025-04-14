cmake_minimum_required(VERSION 3.16)

project(jpoPRO VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    Network
    Charts
)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appjpoPRO
    main.cpp
    api_client.cpp
    data_manager.cpp
)

qt_add_qml_module(appjpoPRO
    URI jpoPRO
    VERSION 1.0
    QML_FILES Main.qml
    RESOURCES qml.qrc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appjpoPRO PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appjpoPRO
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
    Qt6::Quick
    Qt6::Network
    Qt6::Charts
)

include(GNUInstallDirs)
install(TARGETS appjpoPRO
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
