#include <common/base/logger/LoggerManager.h>

using namespace CBASE_LOGGER_NAMESPACE;

#define LOG_TRAFFIC_TRACE(fmt) LOG4CXX_TRACE(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("traffic"), fmt)
#define LOG_TRAFFIC_DEBUG(fmt) LOG4CXX_DEBUG(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("traffic"), fmt)
#define LOG_TRAFFIC_INFO(fmt) LOG4CXX_INFO(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("traffic"), fmt)
#define LOG_TRAFFIC_WARN(fmt) LOG4CXX_WARN(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("traffic"), fmt)
#define LOG_TRAFFIC_ERROR(fmt) LOG4CXX_ERROR(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("traffic"), fmt)
#define LOG_TRAFFIC_FATAL(fmt) LOG4CXX_FATAL(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("traffic"), fmt)

#define LOG_ACCESS_TRACE(fmt) LOG4CXX_TRACE(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access"), fmt)
#define LOGT_ACCESS_TRACE(tag, fmt) do {\
        LOG4CXX_TRACE(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access", tag), fmt);\
     } while (0)

#define LOG_ACCESS_DEBUG(fmt) LOG4CXX_DEBUG(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access"), fmt)
#define LOGT_ACCESS_DEBUG(tag, fmt) do {\
        LOG4CXX_DEBUG(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access", tag), fmt);\
     } while (0)

#define LOG_ACCESS_INFO(fmt) LOG4CXX_INFO(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access"), fmt)
#define LOGT_ACCESS_INFO(tag, fmt) do {\
        LOG4CXX_INFO(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access", tag), fmt);\
     } while (0)

#define LOG_ACCESS_WARN(fmt) LOG4CXX_WARN(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access"), fmt)
#define LOGT_ACCESS_WARN(tag, fmt) do {\
        LOG4CXX_WARN(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access", tag), fmt);\
     } while (0)

#define LOG_ACCESS_ERROR(fmt) LOG4CXX_ERROR(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access"), fmt)
#define LOGT_ACCESS_ERROR(tag, fmt) do {\
        LOG4CXX_ERROR(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access", tag), fmt);\
     } while (0)

#define LOG_ACCESS_FATAL(fmt) LOG4CXX_FATAL(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access"), fmt)
#define LOGT_ACCESS_FATAL(tag, fmt) do {\
        LOG4CXX_FATAL(CBASE_LOGGER_NAMESPACE::LoggerManager::getLogger("access", tag), fmt);\
     } while (0)

int main(int argc, const char* argv[])
{
    LoggerParameter logger_traffic_param;
    logger_traffic_param.module_tag = "traffic";
    logger_traffic_param.file_path = "/var/log/traffic";
    LoggerSPtr logger_traffic = std::make_shared<Logger>();
    logger_traffic->init(logger_traffic_param);

    LoggerParameter logger_access_param;
    logger_access_param.module_tag = "access";
    logger_access_param.file_path = "/var/log/access";
    LoggerSPtr logger_access = std::make_shared<Logger>();
    logger_access->init(logger_access_param);

    LoggerParameter logger_SysLog_param;
    logger_SysLog_param.module_tag = "SysLog";
    logger_SysLog_param.file_path = "/var/log/SysLog";
    logger_SysLog_param.level = "TRACE";
    LoggerSPtr logger_SysLog = std::make_shared<Logger>();
    logger_SysLog->init(logger_SysLog_param);

    LoggerManager::getInstance()->addLogger(logger_traffic);
    LoggerManager::getInstance()->addLogger(logger_access);
    LoggerManager::getInstance()->addLogger(logger_SysLog);

    for (int i = 0; i != 5; i++)
    {
        LOG_TRAFFIC_INFO("this is traffic logger");

        LOG_ACCESS_INFO("this is access logger");
        LOGT_ACCESS_INFO("accsss_tag_1", "this is access logger");
        LOGT_ACCESS_INFO("accsss_tag_2", "this is access logger");

        LOG_SYS_TRACE("TRACE, this log generate by LoggerManager");
        LOGT_SYS_TRACE("sys_tag", "[TRACE] level, this log generate by LoggerManager");

        LOG_SYS_DEBUG("DEBUG, this log generate by LoggerManager");
        LOGT_SYS_DEBUG("sys_tag", "[DEBUG] level, this log generate by LoggerManager");

        LOG_SYS_INFO("INFO, this log generate by LoggerManager");
        LOGT_SYS_INFO("sys_tag", "[INFO] level, this log generate by LoggerManager");

        LOG_SYS_WARN("WARN, this log generate by LoggerManager");
        LOGT_SYS_WARN("sys_tag", "[WARN] level, this log generate by LoggerManager");

        LOG_SYS_ERROR("ERROR, this log generate by LoggerManager");
        LOGT_SYS_ERROR("sys_tag", "[ERROR] level, this log generate by LoggerManager");
    }

    return 0;
}
