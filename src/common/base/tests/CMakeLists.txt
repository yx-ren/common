FILE(GLOB LOGGER_TEST_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
FILE(GLOB LOGGER_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )
ADD_EXECUTABLE(logger_test
    ./logger_test.cpp
    )

ADD_EXECUTABLE(single_obj_test
    ./single_obj_test.cpp
    )

TARGET_LINK_LIBRARIES(logger_test "cbase")
TARGET_LINK_LIBRARIES(single_obj_test "cbase")
