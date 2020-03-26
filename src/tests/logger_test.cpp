#include <iostream>
#include <ostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <logger/LogLite.h>
#include <logger/LogHelper.h>

using namespace LOG_LITE_NAMESPACE;
void processLogItem(LogLitePtr logger, int* max_items, std::mutex* mutex)
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(*mutex);

        std::ostringstream oss;
        oss << std::this_thread::get_id();
        std::string tid = oss.str();

        int left_items = *max_items;
        if (left_items == 0)
        {
            std::cout << "thread:[" << tid << "] exit......" << std::endl;
            break;
        }
        --(*max_items);

        oss.str("");
        oss << "now is:[" << left_items << "] tick";
        std::string log_item = oss.str();
        logger->writeLog(LOG_LEVEL_INFO, log_item);
    }

    return;
}

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        std::cout << "usage : " << argv[0] << " [log items]" << std::endl;
        return 0;
    }

    int item_size = atoi(argv[1]);
    std::cout << "log items:[" << item_size << "]" << std::endl;

    LogConfig log_conf;
    log_conf.mode = LOG_MODE_FILE;
    log_conf.level = LOG_LEVEL_DEBUG;
    std::cout << LogHelper::configToString(log_conf) << std::endl;

    LogLitePtr logger = std::make_shared<LogLite>(log_conf);
    if (!logger->init())
    {
        std::cerr << "call Logger::init() failed" << std::endl;
        return -1;
    }

    auto time_begin = std::chrono::high_resolution_clock::now();
#if 1
    for (int i = 0; i != item_size; i++)
    {
        std::ostringstream oss;
        oss << "now is:[" << i << "] tick";
        std::string log_info = oss.str();
        logger->writeLog(LOG_LEVEL_INFO, log_info);
    }
#else
    int thread_nums = 1;
    std::vector<std::shared_ptr<std::thread>> logger_threads;
    std::mutex _mutex;
    for (int i = 0; i != thread_nums; i++)
    {
        std::shared_ptr<std::thread> th = std::make_shared<std::thread>(
                std::bind(processLogItem, logger, &item_size, &_mutex));
        logger_threads.push_back(th);
    }

    for (auto th : logger_threads)
        th->join();

#endif


    auto time_end = std::chrono::high_resolution_clock::now();
    auto cost_us = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_begin);
    std::cout << "cost time: " << cost_us.count() << " ms" << std::endl;

    return 0;
}
