#include <logger/LogHelper.h>
#include <sstream>

LOG_LITE_NS_BEIGN

std::map<LOG_LEVEL, std::string> LogHelper::mLevelString =
{
    {LOG_LEVEL_TRACE,   "TRACE"},
    {LOG_LEVEL_DEBUG,   "DEBUG"},
    {LOG_LEVEL_INFO,    "INFO"},
    {LOG_LEVEL_WARN,    "WARN"},
    {LOG_LEVEL_ERROR,   "ERROR"},
    {LOG_LEVEL_FAILED,  "FAILED"},
    {LOG_LEVEL_OFF,     "OFF"},
};

std::string LogHelper::levelToString(LOG_LEVEL level)
{
    const auto it = mLevelString.find(level);
    if (it != mLevelString.end())
        return it->second;

    return "";
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
