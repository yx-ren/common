#ifndef __COMMON_BASE_LOGGER_MANAGER_LOGGER_H__
#define __COMMON_BASE_LOGGER_MANAGER_LOGGER_H__

#include <memory>
#include <vector>
#include <common/base/SingleObject.h>
#include <common/base/logger/Logger.h>

#define LOG_SYS_TRACE(fmt) LOG4CXX_TRACE(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog"), fmt)
#define LOGT_SYS_TRACE(tag, fmt) do {\
        LOG4CXX_TRACE(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog", tag), fmt);\
     } while (0)

#define LOG_SYS_DEBUG(fmt) LOG4CXX_DEBUG(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog"), fmt)
#define LOGT_SYS_DEBUG(tag, fmt) do {\
        LOG4CXX_DEBUG(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog", tag), fmt);\
     } while (0)

#define LOG_SYS_INFO(fmt) LOG4CXX_INFO(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog"), fmt)
#define LOGT_SYS_INFO(tag, fmt) do {\
        LOG4CXX_INFO(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog", tag), fmt);\
     } while (0)

#define LOG_SYS_WARN(fmt) LOG4CXX_WARN(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog"), fmt)
#define LOGT_SYS_WARN(tag, fmt) do {\
        LOG4CXX_WARN(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog", tag), fmt);\
     } while (0)

#define LOG_SYS_ERROR(fmt) LOG4CXX_ERROR(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog"), fmt)
#define LOGT_SYS_ERROR(tag, fmt) do {\
        LOG4CXX_ERROR(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog", tag), fmt);\
     } while (0)

#define LOG_SYS_FATAL(fmt) LOG4CXX_FATAL(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog"), fmt)
#define LOGT_SYS_FATAL(tag, fmt) do {\
        LOG4CXX_FATAL(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("SysLog", tag), fmt);\
     } while (0)

CBASE_LOGGER_BEGIN

class LoggerManager : public SingleObject<LoggerManager>
{
public:
    virtual ~LoggerManager();

    static void addLogger(const LoggerSPtr& logger);

    static log4cxx::LoggerPtr getLogger(const std::string& loggerName);
    static log4cxx::LoggerPtr getLogger(const std::string& loggerName, const std::string& tag);

    static void removeLogger(const log4cxx::LoggerPtr logger);

private:
    static std::vector<log4cxx::LoggerPtr> mLoggers;
    static std::mutex mMutex;
};

CBASE_LOGGER_END

#endif
