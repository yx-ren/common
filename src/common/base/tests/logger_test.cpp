#include <common/base/logger/Logger.h>

using namespace CBASE_LOGGER_NAMESPACE;

int main(int argc, const char* argv[])
{
    LoggerParameter logger_traffic_param;
    logger_traffic_param.module_tag = "traffic";
    logger_traffic_param.file_path = "/var/log/traffic";
    Logger logger_traffic;
    logger_traffic.init(logger_traffic_param);

    LoggerParameter logger_access_param;
    logger_access_param.module_tag = "access";
    logger_access_param.file_path = "/var/log/access";
    Logger logger_access;
    logger_access.init(logger_access_param);

    for (int i = 0; i != 10; i++)
    {
        LOG4CXX_INFO(logger_traffic.get_logger(), "this is traffic logger");
        LOG4CXX_INFO(logger_access.get_logger(), "this is access logger");
    }

    return 0;
}
