cmake_minimum_required(VERSION 3.22)
project(gprinterClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(FetchContent)

find_package(Qt6 COMPONENTS
        Core
        Widgets
        Network
        REQUIRED)

FetchContent_Declare(pqxxGit
        GIT_REPOSITORY "https://github.com/jtv/libpqxx.git"
        GIT_TAG "origin/master"   # it's much better to use a specific Git revision or Git tag for reproducibility
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/libpqxx"
)

FetchContent_Declare(jsonGit
        GIT_REPOSITORY "https://github.com/nlohmann/json.git"
        GIT_TAG "origin/master"   # it's much better to use a specific Git revision or Git tag for reproducibility
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/json"
)

FetchContent_MakeAvailable(pqxxGit jsonGit)

add_executable(gprinterClient src/main.cpp
        src/gCode/GCodeParser.cpp
        src/gCode/GCodeParser.h
        src/utils/helpers.h
        src/ui/mainwindow/mainwindow.cpp
        src/ui/mainwindow/mainwindow.h
        src/ui/mainwindow/mainwindow.ui
        src/ui/databaseConnectionDialog/databaseconnectiondialog.cpp
        src/ui/databaseConnectionDialog/databaseconnectiondialog.h
        src/ui/databaseConnectionDialog/databaseconnectiondialog.ui)

target_link_libraries(gprinterClient
        Qt::Core
        Qt::Widgets
        Qt::Network
        nlohmann_json
        pqxx
)

if (DEFAULT_GCODE_PATH)
    target_compile_definitions(gprinterClient PRIVATE DEFAULT_GCODE_PATH="${DEFAULT_GCODE_PATH}")
endif()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Widgets Gui Network)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
    foreach (PQ_LIB pq crypto-3-x64 ssl-3-x64 intl-9 winpthread-1 iconv-2)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${PostgreSQL_ROOT}/bin/lib${PQ_LIB}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (PQ_LIB)
endif ()
