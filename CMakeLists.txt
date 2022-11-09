cmake_minimum_required(VERSION 3.5)

project(Accounting VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(AWSSDK REQUIRED COMPONENTS s3)
find_package(fmt REQUIRED)

set(PROJECT_SOURCES
    Widgets/BillListWidget.hpp
    Widgets/BillEditorDialog.hpp
    Widgets/TransactionEditorDialog.hpp
    Models/BillListModel.hpp
    Models/BillListModel.cpp
    Models/DatabaseModel.hpp
    Models/TransactionListModel.hpp
    Models/TransactionListModel.cpp
    Models/PocketMoneyProxyModel.hpp
    Models/PocketMoneyProxyModel.cpp
    Models/BillProxyModel.hpp
    Models/BillProxyModel.cpp
    Models/TransactionModel.hpp
    Persistance/Database.hpp
    Persistance/S3.hpp
    MainWindow.hpp
    Util.hpp
    main.cpp

    Notes/0001_form-layout-expands.md
    Notes/0002_understanding-model-view.md
    Notes/0003_generate-spreadsheet-odf.md

    Scripts/backup.sh

    LICENSE
    README.md
    TODO.md
)

qt_wrap_ui(PROJECT_SOURCES
    Widgets/TransactionEditorDialog.ui
    Widgets/BillEditorDialog.ui
    Widgets/BillListWidget.ui
    MainWindow.ui)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Accounting
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Accounting APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Accounting SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Accounting
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Accounting PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${AWSSDK_LINK_LIBRARIES} fmt::fmt)

set_target_properties(Accounting PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER accounting.apps.asynts.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Accounting
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Accounting)
endif()
