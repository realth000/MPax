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
endif ()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Multimedia LinguistTools Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Multimedia LinguistTools Sql)

include_directories(src)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MPax MANUAL_FINALIZATION ${PROJECT_SOURCES} ${QM_FILES} ${TS_FILES})
else ()
    add_executable(MPax)
endif ()

add_subdirectory(resource)
add_subdirectory(src)

set(TS_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/resource/translation/zh_CN.ts
        ${CMAKE_CURRENT_SOURCE_DIR}/resource/translation/en.ts)

find_program(LUPDATE_EXECUTABLE lupdate)
find_program(LRELEASE_EXECUTABLE lrelease)

foreach (_ts_file ${TS_FILES})
    execute_process(
            COMMAND ${LUPDATE_EXECUTABLE} -recursive ${CMAKE_SOURCE_DIR} -ts ${_ts_file})
    execute_process(
            COMMAND ${LRELEASE_EXECUTABLE} ${_ts_file})
endforeach ()

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_SOURCE_DIR}/resource/translation")
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

#qt5_add_translation(QM_FILES ${TS_FILES})

target_link_libraries(MPax PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Sql)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MPax)
endif ()
