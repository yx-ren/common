#include <iostream>
#include <ostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <logger/LogLite.h>
#include <logger/LogHelper.h>

int main(int argc, const char* argv[])
{
    using namespace LOG_LITE_NAMESPACE;
    LogConfig log_conf;
    log_conf.mode = LOG_MODE_FILE;
    log_conf.level = LOG_LEVEL_DEBUG;
    log_conf.file_size = 1024 * 1024;
    LogLite logger(log_conf);
    std::cout << LogHelper::configToString(log_conf) << std::endl;
    if (!logger.init())
    {
        std::cerr << "call Logger::init() failed" << std::endl;
        return -1;
    }

    auto time_begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i != 500000; i++)
    {
        std::ostringstream oss;
        oss << "now is:[" << i << "] tick";
        std::string log_info = oss.str();
        logger.writeLog(LOG_LEVEL_INFO, log_info);
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto cost_us = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_begin);
    std::cout << "cost time:[" << cost_us.count() << "] ms" << std::endl;

    return 0;
}
