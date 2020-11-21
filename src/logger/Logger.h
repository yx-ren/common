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
    bool syslog_flag;
    std::string syslog_ip;

};
typedef std::shared_ptr<LoggerParamer> LoggerParamerSPtr;

template<typename T>
class ILogger
{
public:
    virtual ~ILogger() {}

    virtual void init() = 0;

    virtual void init(const LoggerParamer& param) = 0;

    virtual void log(T msg) = 0;

private:
    LoggerParamer mParam;
    std::shared_ptr<ILogger> mLogger;
};
typedef std::shared_ptr<ILogger> ILoggerSPtr;

class Logger4cxx : public ILogger
{
public:
    virtual ~Logger4cxx() {}

    virtual void init() override;

    virtual void init(const LoggerParamer& param) override;
};
typedef std::shared_ptr<Logger4cxx> Logger4cxxSPtr;

class Glogger : public ILogger
{
    virtual ~Glogger() {}

    virtual void init() override;

    virtual void init(const LoggerParamer& param) override;
};
typedef std::shared_ptr<Glogger> GloggerSPtr;

SGLD_END

#endif
