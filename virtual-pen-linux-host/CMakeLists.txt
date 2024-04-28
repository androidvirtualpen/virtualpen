cmake_minimum_required(VERSION 3.5)

project(virtual-pen-linux-host VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp        
        mainwindow.h
        mainwindow.ui
        udevdialog.ui
        accessory.cpp
        hid.cpp
        linux-adk.cpp
)

add_executable(virtual-pen-linux-host ${PROJECT_SOURCES}
    error.c
    error.h
    uinput.c
    log.c
    log.h
    uinput.h
    accessory.h
    virtualstylus.h virtualstylus.cpp
    linux-adk.h
    constants.h
    displayscreentranslator.h
    displayscreentranslator.cpp
    pressuretranslator.h pressuretranslator.cpp
    icon.qrc
    filepermissionvalidator.h filepermissionvalidator.cpp
    udevdialog.ui

)

target_link_libraries(virtual-pen-linux-host PRIVATE Qt${QT_VERSION_MAJOR}::Widgets usb-1.0)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.pen-emulator-linux-host)
endif()
set_target_properties(virtual-pen-linux-host PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS virtual-pen-linux-host
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES resources/io.github.androidvirtualpen.virtualpen.desktop DESTINATION /usr/share/applications/)
install(FILES resources/io.github.androidvirtualpen.virtualpen.svg DESTINATION /usr/share/icons/hicolor/scalable/apps/)
install(FILES resources/io.github.androidvirtualpen.virtualpen.metainfo.xml DESTINATION /usr/share/metainfo/)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(virtual-pen-linux-host)
endif()
