cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE SYNTHCORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(synthcore_core STATIC ${SYNTHCORE_SOURCES})

target_include_directories(synthcore_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(synthcore_core PUBLIC cxx_std_20)

if(SYNTHCORE_BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    add_executable(synthcore_tests ${TEST_SOURCES})

    target_link_libraries(synthcore_tests
        PRIVATE
            synthcore_core
            GTest::gtest
            GTest::gtest_main
    )

    target_include_directories(synthcore_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )

    gtest_discover_tests(synthcore_tests)
endif()
