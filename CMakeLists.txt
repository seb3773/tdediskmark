cmake_minimum_required(VERSION 3.10)

project(tdediskmark VERSION 3.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -flto -s -ffunction-sections -fdata-sections -fno-stack-protector -fvisibility=hidden -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-ident -fno-plt -fomit-frame-pointer -ffast-math -fmerge-all-constants")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,--as-needed -Wl,--build-id=none -Wl,--strip-all -Wl,--icf=all -fuse-ld=gold")

find_package(PkgConfig REQUIRED)
pkg_check_modules(TQT REQUIRED tqt)
pkg_check_modules(TDE REQUIRED tdelibs)
pkg_check_modules(NOTIFY REQUIRED libnotify)

include_directories(
    /opt/trinity/include
    ${TQT_INCLUDE_DIRS}
    ${TDE_INCLUDE_DIRS}
    ${NOTIFY_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/aa-painter
    ${CMAKE_CURRENT_SOURCE_DIR}/meter-gauge
)

link_directories(
    /opt/trinity/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mainwindow.moc.cpp
    COMMAND /usr/bin/tqmoc ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.h -o ${CMAKE_CURRENT_BINARY_DIR}/mainwindow.moc.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/benchmark.moc.cpp
    COMMAND /usr/bin/tqmoc ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.h -o ${CMAKE_CURRENT_BINARY_DIR}/benchmark.moc.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/appsettings.moc.cpp
    COMMAND /usr/bin/tqmoc ${CMAKE_CURRENT_SOURCE_DIR}/src/appsettings.h -o ${CMAKE_CURRENT_BINARY_DIR}/appsettings.moc.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/appsettings.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/settings.moc.cpp
    COMMAND /usr/bin/tqmoc ${CMAKE_CURRENT_SOURCE_DIR}/src/settings.h -o ${CMAKE_CURRENT_BINARY_DIR}/settings.moc.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/settings.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/aboutdialog.moc.cpp
    COMMAND /usr/bin/tqmoc ${CMAKE_CURRENT_SOURCE_DIR}/src/aboutdialog.h -o ${CMAKE_CURRENT_BINARY_DIR}/aboutdialog.moc.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/aboutdialog.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tqtmetergauge.moc.cpp
    COMMAND /usr/bin/tqmoc ${CMAKE_CURRENT_SOURCE_DIR}/meter-gauge/tqtmetergauge_aa.h -o ${CMAKE_CURRENT_BINARY_DIR}/tqtmetergauge.moc.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/meter-gauge/tqtmetergauge_aa.h)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/mainwindow.cpp
    src/benchmark.cpp
    src/appsettings.cpp
    src/global.cpp
    src/diskdriveinfo.cpp
    src/settings.cpp
    src/aboutdialog.cpp
    meter-gauge/tqtmetergauge_aa.cpp
    aa-painter/tqtaapainter.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/mainwindow.moc.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark.moc.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/appsettings.moc.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/settings.moc.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/aboutdialog.moc.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/tqtmetergauge.moc.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${TQT_LIBRARIES}
    ${TDE_LIBRARIES}
    ${NOTIFY_LIBRARIES}
    tdecore
    tdeui
    tdesu
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND strip --strip-all --remove-section=.comment --remove-section=.note $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND sstrip $<TARGET_FILE:${PROJECT_NAME}>
)
