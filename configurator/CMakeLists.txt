    cmake_minimum_required(VERSION 3.5)

    project(configurator VERSION 0.1 LANGUAGES CXX)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../build-debug)

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)


    set(CMAKE_PREFIX_PATH "C:\\Qt\\6.6.2\\mingw_64\\lib\\cmake\\")

    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SerialPort)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

    set(PROJECT_SOURCES
            main.cpp
            mainwindow.cpp
            mainwindow.h
            mainwindow.ui
    )

    #qt6_wrap_ui(SOURCES mainwindow.ui)

    qt_add_executable(configurator
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES} ${SOURCES} ${UI_HEADERS}
    )

    target_link_libraries(configurator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
    target_link_libraries(configurator PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
    target_link_libraries(configurator PRIVATE Qt${QT_VERSION_MAJOR}::Test)
    # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
    # If you are developing for iOS or macOS you should consider setting an
    # explicit, fixed bundle identifier manually though.
    if(${QT_VERSION} VERSION_LESS 6.1.0)
        set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.configurator)
    endif()
    set_target_properties(configurator PROPERTIES
            ${BUNDLE_ID_OPTION}
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
            MACOSX_BUNDLE TRUE
            WIN32_EXECUTABLE TRUE
    )

    include(GNUInstallDirs)
    install(TARGETS configurator
            BUNDLE DESTINATION .
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(configurator)
    endif()

