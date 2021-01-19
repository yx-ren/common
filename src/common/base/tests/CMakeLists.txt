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

ADD_EXECUTABLE(config_parser_test
    ./ConfigParserTest.cpp
    )

ADD_EXECUTABLE(time_test
    ./TimeTest.cpp
    )

ADD_EXECUTABLE(app_main_test
    ./AppMainTest.cpp
    )

TARGET_LINK_LIBRARIES(logger_test "cbase")
TARGET_LINK_LIBRARIES(single_obj_test "cbase")
TARGET_LINK_LIBRARIES(config_parser_test "cbase")
TARGET_LINK_LIBRARIES(time_test "cbase")
TARGET_LINK_LIBRARIES(app_main_test "cbase")

file(COPY "./etc/test.xml" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/etc)
