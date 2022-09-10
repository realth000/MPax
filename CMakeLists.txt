cmake_minimum_required(VERSION 3.13)

project(MPax VERSION 1.4.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    if ((CMAKE_BUILD_TYPE STREQUAL "Debug"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O0 -g -fno-inline")
        message(">>> debug mode")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -s -Wl,-z,relro,-z,now,-z,noexecstack -fstack-protector-all -fPIE -pie -D_FORTIFY_SOURCE=2")
        message(">>> release mode")
    endif ()
endif ()

if (UNIX)
    find_package(X11 REQUIRED)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS X11Extras)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS X11Extras)
endif ()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Multimedia LinguistTools Sql Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Multimedia LinguistTools Sql Concurrent)

include_directories(external)
include_directories(external/taglib)
include_directories(external/taglib/taglib)
include_directories(src)
# Should move this include directories to taglib
include_directories(
        external/taglib/taglib/toolkit
        external/taglib/taglib/asf
        external/taglib/taglib/mpeg
        external/taglib/taglib/ogg
        external/taglib/taglib/ogg/flac
        external/taglib/taglib/flac
        external/taglib/taglib/mpc
        external/taglib/taglib/mp4
        external/taglib/taglib/ogg/vorbis
        external/taglib/taglib/ogg/speex
        external/taglib/taglib/ogg/opus
        external/taglib/taglib/mpeg/id3v2
        external/taglib/taglib/mpeg/id3v2/frames
        external/taglib/taglib/mpeg/id3v1
        external/taglib/taglib/ape
        external/taglib/taglib/wavpack
        external/taglib/taglib/trueaudio
        external/taglib/taglib/riff
        external/taglib/taglib/riff/aiff
        external/taglib/taglib/riff/wav
        external/taglib/taglib/mod
        external/taglib/taglib/s3m
        external/taglib/taglib/it
        external/taglib/taglib/xm
        external/taglib/3rdparty
)
#get_property(dirs DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#message(">>> include_dirs=${dirs}")

# Build static
add_definitions(-DTAGLIB_STATIC)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MPax MANUAL_FINALIZATION ${PROJECT_SOURCES} ${QM_FILES} ${TS_FILES})
else ()
    add_executable(MPax)
endif ()

add_library(qhotkey)
add_library(QHotkey::QHotkey ALIAS qhotkey)

add_subdirectory(external)
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

target_link_libraries(MPax PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Sql qhotkey tag z Qt${QT_VERSION_MAJOR}::Concurrent)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MPax)
endif ()

# Get git info
find_package(Git QUIET)
if (GIT_FOUND)
    set(GIT_COMMIT_TIME "")
    set(GIT_COMMIT "")
    set(GIT_COMMIT_LONG "")

    # GIT_BRANCH   = $$system(git rev-parse --abbrev-ref HEAD)
    execute_process(
            #COMMAND ${GIT_EXECUTABLE} --no-pager log -1 --pretty=format:%cd --date=format:"%F %T %z"
            COMMAND ${GIT_EXECUTABLE} --no-pager show --oneline --format=%cd --date=format:"%F %T %z" -s HEAD
            OUTPUT_VARIABLE GIT_COMMIT_TIME
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
            COMMAND ${GIT_EXECUTABLE} --no-pager show --oneline --format=%h -s HEAD
            OUTPUT_VARIABLE GIT_COMMIT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
            COMMAND ${GIT_EXECUTABLE} --no-pager show --oneline --format=%H -s HEAD
            OUTPUT_VARIABLE GIT_COMMIT_LONG
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --tags
            OUTPUT_VARIABLE GIT_VERSION_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
            COMMAND ${GIT_EXECUTABLE} log --pretty=oneline ${GIT_VERSION_TAG}...${GIT_COMMIT}
            COMMAND wc -l
            COMMAND xargs expr 1 +
            OUTPUT_VARIABLE GIT_RELEASE_COUNT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            #            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_definitions(-DAPP_COMMIT_TIME=${GIT_COMMIT_TIME})
    add_definitions(-DAPP_COMMIT="${GIT_COMMIT}")
    add_definitions(-DAPP_COMMIT_LONG="${GIT_COMMIT_LONG}")
    add_definitions(-DAPP_VERSION_TAG="${GIT_VERSION_TAG}")
    add_definitions(-DAPP_RELEASE_COUNT="${GIT_RELEASE_COUNT}")
    message(STATUS "Git commit time:" ${GIT_COMMIT_TIME})
    message(STATUS "Git commit:" ${GIT_COMMIT})
    message(STATUS "Git commit full name:" ${GIT_COMMIT_LONG})
    message(STATUS "Git version tag:" ${GIT_VERSION_TAG})
    message(STATUS "Git release count:" ${GIT_RELEASE_COUNT})
else ()
    message(WARNING "Git not found")
endif ()
