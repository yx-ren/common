SET(target "cbase")
#ADD_SUBDIRECTORY(logger)
#ADD_SUBDIRECTORY(test)

FILE(GLOB LOGGER_HEADERS ./logger/*.h)
FILE(GLOB LOGGER_SOURCES ./logger/*.cpp)

ADD_LIBRARY(${target} SHARED ${LOGGER_HEADERS} ${LOGGER_SOURCES})
TARGET_LINK_LIBRARIES(${target} log4cxx)

#SET(DETOUR_SDK_HEADRES
#    SGDetour.h
#    )
#
#INSTALL(FILES ${DETOUR_SDK_HEADRES} DESTINATION include/skyguard/LinuxDetours/detours/sdk COMPONENT dev)
#INSTALL(TARGETS ${target} DESTINATION lib COMPONENT dev)
#INSTALL(TARGETS ${target}_static DESTINATION lib COMPONENT dev)
#
## copy 3rd(glog)
#INSTALL(DIRECTORY /usr/local/include/gflags /usr/local/include/glog DESTINATION include USE_SOURCE_PERMISSIONS COMPONENT dev)
#INSTALL(FILES /usr/local/lib/libglog.so.0.0.0 DESTINATION lib COMPONENT dev)

ADD_SUBDIRECTORY(tests)
