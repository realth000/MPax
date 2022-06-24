cmake_minimum_required(VERSION 3.13)

project(MPax VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -s -Wl,-z,relro,-z,now,-z,noexecstack -fstack-protector-all -fPIE -pie -D_FORTIFY_SOURCE=2")
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Multimedia)

include_directories(src)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MPax MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(MPax)
endif()

add_subdirectory(resource)
add_subdirectory(src)

target_link_libraries(MPax PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MPax)
endif()
