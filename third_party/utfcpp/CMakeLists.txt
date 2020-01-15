cmake_minimum_required (VERSION 3.0.2)
project (utf8cpp VERSION 3.1 LANGUAGES CXX)

option(UTF8_TESTS "Enable tests for UTF8-CPP" On)
option(UTF8_INSTALL "Enable installation for UTF8-CPP" On)
option(UTF8_SAMPLES "Enable building samples for UTF8-CPP" On)

add_library(utf8cpp INTERFACE)
target_include_directories(utf8cpp INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>"
    $<INSTALL_INTERFACE:include/utf8cpp>
)
add_library(utf8::cpp ALIAS utf8cpp)

if(UTF8_INSTALL)
    if(MSVC)
        set(DEF_INSTALL_CMAKE_DIR CMake)
    else()
        include(GNUInstallDirs) # define CMAKE_INSTALL_*
        set(DEF_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/utf8cpp)
    endif()

    install(DIRECTORY source/ DESTINATION include/utf8cpp)
    install(TARGETS utf8cpp EXPORT utf8cppConfig)
    install(EXPORT utf8cppConfig DESTINATION ${DEF_INSTALL_CMAKE_DIR})
endif()

if(UTF8_SAMPLES)
    add_executable(docsample ${PROJECT_SOURCE_DIR}/samples/docsample.cpp)
    target_link_libraries(docsample PRIVATE utf8::cpp)
endif()

if(UTF8_TESTS)
    enable_testing()
    add_subdirectory(extern/gtest)
    add_subdirectory(tests)
endif()
