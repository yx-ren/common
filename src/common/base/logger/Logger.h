#ifndef __COMMON_BASE_LOGGER_LOGGER_H__
#define __COMMON_BASE_LOGGER_LOGGER_H__

#include <memory>
#include <log4cxx/logger.h>
#include <common/base/logger/common.h>

CBASE_LOGGER_BEGIN

struct LoggerParameter
{
    LoggerParameter()
    {
        processor_tag = "sys";
        module_tag = "MC";
        level = "TRACE";
        console_flag = true;
        file_flag = true;
        file_path = "/var/log/dlp";
        file_size = "1024000";
        file_backup = "4";
    }
    std::string processor_tag;
    std::string module_tag;
    std::string level;
    bool console_flag;
    bool file_flag;
    std::string file_path;
    std::string file_size;
    std::string file_backup;
};
typedef std::shared_ptr<LoggerParameter> LoggerParameterSPtr;

class Logger
{
public:
    void init();

    void init(const LoggerParameter& param);

    log4cxx::LoggerPtr get_logger() const;

private:
    log4cxx::LoggerPtr mLogger;
};
typedef std::shared_ptr<Logger> LoggerSPtr;

CBASE_LOGGER_END

#endif
