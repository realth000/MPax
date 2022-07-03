target_include_directories(qhotkey PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(qhotkey PRIVATE qhotkey.h qhotkey_p.h qhotkey.cpp)
target_link_libraries(qhotkey Qt5::Core Qt5::Widgets)

if (WIN32)
    target_sources(qhotkey PRIVATE qhotkey_win.cpp)
else ()
    include_directories(${X11_INCLUDE_DIR})
    target_sources(qhotkey PRIVATE qhotkey_x11.cpp)
    target_link_libraries(qhotkey ${X11_LIBRARIES} Qt5::X11Extras)
endif ()
