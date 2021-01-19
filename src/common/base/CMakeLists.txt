SET(target "cbase")

FILE(GLOB BASE_HEADERS ./*.h)
FILE(GLOB BASE_SOURCES ./*.cpp)

FILE(GLOB LOGGER_HEADERS ./logger/*.h)
FILE(GLOB LOGGER_SOURCES ./logger/*.cpp)


include_directories(${PROJECT_SOURCE_DIR}/3rdparty/backtrace)

ADD_LIBRARY(${target} SHARED
    ${BASE_HEADERS} ${BASE_SOURCES}
    ${LOGGER_HEADERS} ${LOGGER_SOURCES}
    ${PROJECT_SOURCE_DIR}/3rdparty/backtrace/libs/backtrace/src/backtrace.cpp
    )
TARGET_LINK_LIBRARIES(${target} ${Log4CXX_LIBRARIES})
TARGET_LINK_LIBRARIES(${target} ${Boost_LIBRARIES})

INSTALL(FILES "cbase_export.h" DESTINATION include/common/base/ COMPONENT dev)
INSTALL(FILES ${BASE_HEADERS} DESTINATION include/common/base/ COMPONENT dev)
INSTALL(FILES ${LOGGER_HEADERS} DESTINATION include/common/base/logger COMPONENT dev)
INSTALL(TARGETS ${target} DESTINATION lib COMPONENT dev)


include(GenerateExportHeader)
add_compiler_export_flags()

generate_export_header(${target}
  BASE_NAME ${sgbase}
  EXPORT_MACRO_NAME CBASE_EXPORT
  EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/cbase_export.h"
)

ADD_SUBDIRECTORY(tests)
