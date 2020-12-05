#ifndef __SG_LOGGER_H__
#define __SG_LOGGER_H__

#include <memory>
#include <skyguard/LinuxDetours/common.h>

SGLD_BEGIN

struct LoggerParamer
{
    LoggerParamer()
    {
        module_tag = "MC";
        level = "TRACE";
        console_flag = true;
        file_flag = true;
        file_path = "/var/log/dlp";
        file_size = "1024000";
        file_backup = "4";
        syslog_flag = true;
        syslog_ip = "127.0.0.1";
    }
    std::string module_tag;
    std::string level;
    bool console_flag;
    bool file_flag;
    std::string file_path;
    std::string file_size;
    std::string file_backup;
};
typedef std::shared_ptr<LoggerParamer> LoggerParamerSPtr;

class Logger
{
public:
    void init();

    void init(const LoggerParamer& param);

private:
    log4cxx::LoggerPtr mLogger;
};
typedef std::shared_ptr<Logger> LoggerSPtr;


SGLD_END

#endif
