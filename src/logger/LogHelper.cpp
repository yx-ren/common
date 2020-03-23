#include <logger/LogHelper.h>
#include <sstream>

LOG_LITE_NS_BEIGN

std::string LogHelper::levelToString(LOG_LEVEL level)
{
    std::string log_level("");
    switch(level)
    {
        case LOG_LEVEL_TRACE:
            log_level = "TRACE";
            break;
        case LOG_LEVEL_DEBUG:
            log_level = "DEBUG";
            break;
        case LOG_LEVEL_INFO:
            log_level = "INFO";
            break;
        case LOG_LEVEL_WARN:
            log_level = "WARN";
            break;
        case LOG_LEVEL_ERROR:
            log_level = "ERROR";
            break;
        case LOG_LEVEL_FAILED:
            log_level = "FAILED";
            break;
        case LOG_LEVEL_OFF:
            log_level = "OFF";
            break;
        default:
            log_level = "UNKNOWN";
            break;
    }

    return log_level;
}

std::string LogHelper::modeToString(int mode)
{
    std::string log_mode;
    if (mode & LOG_MODE_FILE)
        log_mode += "FILE; ";
    if (mode & LOG_MODE_CONSOLE)
        log_mode += "CONSOLE; ";

    return log_mode;
}

std::string LogHelper::configToString(LogConfig conf)
{
    std::ostringstream oss;
    oss << "log level:[" << levelToString(conf.level) << "], "
        << "log mode:[" << modeToString(conf.mode) << "], "
        << "log path:[" << conf.file_path << "], "
        << "log size:[" << conf.file_size << "] bytes, "
        << "log backup:[" << conf.file_backup<< "]"
        << std::endl;

    return oss.str();
}

LOG_LITE_NS_END
