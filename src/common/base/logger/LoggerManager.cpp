#include <common/base/logger/LoggerManager.h>
#include <algorithm>
#include <iostream>

CBASE_LOGGER_BEGIN

std::vector<log4cxx::LoggerPtr> LoggerManager::mLoggers =
    {
        log4cxx::Logger::getLogger("SysLog"),
        //log4cxx::Logger::getLogger("traffic"),
    };
std::mutex LoggerManager::mMutex;

LoggerManager::~LoggerManager()
{
}

void LoggerManager::addLogger(const LoggerSPtr& logger)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mLoggers.push_back(logger->getRawLogger());
}

log4cxx::LoggerPtr LoggerManager::getLogger(const std::string& loggerName)
{
    std::unique_lock<std::mutex> lock(mMutex);
    auto logger = std::find_if(mLoggers.begin(), mLoggers.end(),
            [&](const log4cxx::LoggerPtr logger) { return logger->getName() == loggerName;});
    return logger != mLoggers.end() ? *logger : log4cxx::LoggerPtr();
}

log4cxx::LoggerPtr LoggerManager::getLogger(const std::string& loggerName, const std::string& tag)
{
    std::unique_lock<std::mutex> lock(mMutex);
    auto logger = std::find_if(mLoggers.begin(), mLoggers.end(),
            [&](const log4cxx::LoggerPtr logger) { return logger->getName() == loggerName;});

    log4cxx::LoggerPtr found_logger;
    if (logger != mLoggers.end())
    {
        std::string new_tag = loggerName + "." + tag;
        found_logger = (*logger)->getLogger(new_tag.c_str());
    }
    return found_logger;
}

void LoggerManager::removeLogger(const log4cxx::LoggerPtr logger)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mLoggers.erase(std::remove(mLoggers.begin(), mLoggers.end(), logger), mLoggers.end());
}

CBASE_LOGGER_END
