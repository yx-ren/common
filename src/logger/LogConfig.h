#ifndef __LOG_LITE_LOG_CONFIG_H__
#define __LOG_LITE_LOG_CONFIG_H__

#include <string>
#include <logger/common.h>

LOG_LITE_NS_BEIGN

enum LOG_LEVEL
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FAILED,
    LOG_LEVEL_OFF,
    LOG_LEVEL_UNKNOWN,
};

enum LOG_MODE
{
    LOG_MODE_FILE = 1 << 0,
    LOG_MODE_CONSOLE = 1 << 1,
};

struct LogConfig
{
    LogConfig()
        : level(LOG_LEVEL_TRACE)
        , mode(LOG_MODE_FILE | LOG_MODE_CONSOLE)
        , file_path("logLite")
        , file_size(1024 * 1024 * 10)
        , file_backup(10)
        , mod_tag("log_lite")
        , log_tag("sys_log")
    {}

    LOG_LEVEL level;
    int mode;
    std::string file_path;
    size_t file_size;
    size_t file_backup;
    std::string mod_tag;
    std::string log_tag;
};

LOG_LITE_NS_END

#endif
