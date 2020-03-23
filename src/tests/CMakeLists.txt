FILE(GLOB LOG_LITE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../logger/*.h
    )
FILE(GLOB LOG_LITE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../logger/*.cpp
    )
ADD_EXECUTABLE(logger_test
    logger_test.cpp
    ${LOG_LITE_SOURCES}
    ${LOG_LITE_HEADERS}
    )
