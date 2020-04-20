#include <logger/LogLite.h>
#include <time.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <thread>
#include <iterator>
#if defined(WIN32) || defined(_WIN32)
#include <mutex>
#else
#include <condition_variable>
#endif

#include <logger/FileHelper.h>
//#include <logger/BlockTaskQueue.h>

LOG_LITE_NS_BEIGN

#define FILE_BLOCK_SIZE 4096
#define LOG_ITEM_BUFF_SIZE 100
#define IO_THREAD_INTERVAL_SECOND 3
#define PROCESS_RAW_LOG_ITEM_THREADS_NUMBER 4
#define PROCESS_FORMAT_LOG_ITEM_THREADS_NUMBER 1

struct LogMetaInfo
{
    LOG_LEVEL level;
    std::string msg;
    //std::thread::id tid;
    std::string tid;
};
typedef std::shared_ptr<LogMetaInfo> LogMetaInfoPtr;

// ---------------------------------------- //
// LogLiteImpl
// ---------------------------------------- //

class LogLiteImpl
{
public:
    typedef std::pair<LOG_LEVEL, std::string> LogItem;
    typedef std::shared_ptr<LogItem> LogItemPtr;

    explicit LogLiteImpl(const LogConfig& conf)
        : mLogConfig(conf)
        , mLogLevel(LogHelper::levelToString(mLogConfig.level))
        , mLogSize(0)
        , mIsRunning(false)
        , mInitThreads(0)
    {
        reset();
    }

    ~LogLiteImpl();

    bool init(void);

    void setConfig(const LogConfig& conf);
    LogConfig getConfig(void) const;

    void writeLog(LOG_LEVEL level, const std::string& log);

protected:
    virtual inline std::string generatePrefix(void);
    virtual inline std::string generatePrefix(const std::string& tid, const std::string& level); // may be not use lock

    virtual inline std::string generateTimestamp(void);
    virtual inline std::string generateThreadID(void);
    virtual inline std::string& getModuleTag(void);
    virtual inline std::string& getLogLevel(void);
    virtual inline std::string& getLogTag(void);

    void reset(void);

    bool reopen();

    void rollback(void);

    void addLogRawItem(LOG_LEVEL level, const std::string& log);
    void addLogFormatItem(std::shared_ptr<std::string> format_log);

    void processLogRawItem();
    void processLogFormatItem();

private:
    mutable std::mutex mMutex;
    mutable std::mutex mMutexCount;
    LogConfig mLogConfig;
    std::shared_ptr<std::ofstream> mOfs;
    std::string mLogLevel;
    size_t mLogSize;
    bool mIsRunning;
    size_t mInitThreads;

    std::queue<LogMetaInfoPtr> mLogRawItemQueue;
    std::queue<std::shared_ptr<std::string>> mLogFormatItemQueue;

    std::mutex mRawItemQueueMutex;
    std::mutex mFormatItemQueueMutex;

    std::condition_variable mRawItemQueueCond;
    std::condition_variable mFormatItemQueueCond;

    std::vector<std::shared_ptr<std::thread>> mConsumRawThreads;
    std::vector<std::shared_ptr<std::thread>> mConsumFormatThreads;
};

LogLiteImpl::~LogLiteImpl()
{
    {
        std::unique_lock<std::mutex> lk(mMutex);
        mIsRunning = false;
        mRawItemQueueCond.notify_all();
        //mFormatItemQueueCond.notify_all();
    }


    for (auto& th : mConsumRawThreads)
        th->join();

#if 1
    for (auto& th : mConsumFormatThreads)
        th->join();
#endif
}

bool LogLiteImpl::init(void)
{
    std::unique_lock<std::mutex> lk(mMutex);
    if (mOfs)
        return true;

    for (int i = 0; i != PROCESS_RAW_LOG_ITEM_THREADS_NUMBER; i++)
    {
        std::shared_ptr<std::thread> th = std::make_shared<std::thread>(std::bind(&LogLiteImpl::processLogRawItem, this));
        mConsumRawThreads.push_back(th);
    }

    for (int i = 0; i != PROCESS_FORMAT_LOG_ITEM_THREADS_NUMBER; i++)
    {
        std::shared_ptr<std::thread> th = std::make_shared<std::thread>(std::bind(&LogLiteImpl::processLogFormatItem, this));
        mConsumFormatThreads.push_back(th);
    }

    FileHelper::getFileSize(mLogConfig.file_path, mLogSize);
    if (!reopen())
        return false;

    int retry_times = 0;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::unique_lock<std::mutex> lk(mMutexCount);
        if (mInitThreads == PROCESS_RAW_LOG_ITEM_THREADS_NUMBER + PROCESS_FORMAT_LOG_ITEM_THREADS_NUMBER)
        {
            std::cout << "init done" << std::endl;
            break;
        }
        else
        {
            std::cout << "wait thread inited and retry times:[" << ++retry_times << "]" << std::endl;
        }
    }
    mIsRunning = true;

    return true;
}

void LogLiteImpl::setConfig(const LogConfig& conf)
{
    std::unique_lock<std::mutex> lk(mMutex);
    mLogConfig = conf;
    mLogLevel = LogHelper::levelToString(mLogConfig.level);
}

LogConfig LogLiteImpl::getConfig(void) const
{
    std::unique_lock<std::mutex> lk(mMutex);
    return mLogConfig;
}

void LogLiteImpl::writeLog(LOG_LEVEL level, const std::string& log)
{
#if 0
    std::unique_lock<std::mutex> lk(mMutex);

    if (level < mLogConfig.level)
        return;

    if (mLogSize >= mLogConfig.file_size)
    {
        // close
        reset();

        rollback();

        if (!reopen())
            return;
    }

    if (!mOfs)
    {
        std::cerr << "logger not be inited, call init() and try again" << std::endl;
        return;
    }

    std::ostringstream oss;
    std::string log_prefix = generatePrefix();
    oss << log_prefix << log << "\n";

    std::string log_item = oss.str();
    if (mLogConfig.mode & LOG_MODE_FILE)
    {
        *mOfs << log_item;
    }

    if (mLogConfig.mode & LOG_MODE_CONSOLE)
    {
        std::cout << log_item;
    }

    mLogSize += log_item.size();

    return;
#else
    addLogRawItem(level, log);
#endif
}

std::string LogLiteImpl::generatePrefix(void)
{
    std::ostringstream oss;
    oss << generateTimestamp() << " "
        << "[" << generateThreadID() << "] "
        << "<" << getModuleTag() << ">: "
        << getLogLevel() << " "
        << getLogTag() << " "
        << " - ";

    return oss.str();
}

std::string LogLiteImpl::generatePrefix(const std::string& tid, const std::string& level)
{
    std::ostringstream oss;
    oss << generateTimestamp()
        << " [" << tid << "] "
        << " <" << getModuleTag() << ">: "
        << " " << level << " "
        << " " << getLogTag() << " "
        << " - ";

    return oss.str();
}

std::string LogLiteImpl::generateTimestamp(void)
{
    auto time_now    = std::chrono::system_clock::now();

    auto duration_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
    auto part_ms     =
        duration_ms - std::chrono::duration_cast<std::chrono::seconds>(duration_ms);

    auto duration_us =
        std::chrono::duration_cast<std::chrono::microseconds>(time_now.time_since_epoch());
    auto part_us     =
        duration_us - std::chrono::duration_cast<std::chrono::milliseconds>(duration_us);

    struct tm local_time;
    time_t ctime = std::chrono::system_clock::to_time_t(time_now);
#if defined(WIN32) || defined(_WIN32)
    _localtime64_s(&local_time, &ctime);
#else
    localtime_r(&ctime, &local_time);
#endif

    char timestamp[0x7f] = {0};
    std::strftime(timestamp, sizeof(timestamp),
        "%Y-%m-%d %H:%M:%S", &local_time);

    std::ostringstream oss;
    oss << timestamp
        << "," << std::setw(3) << std::setfill('0') << part_ms.count()
        << "," << std::setw(3) << std::setfill('0') << part_us.count();

    return oss.str();
}

std::string LogLiteImpl::generateThreadID(void)
{
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    unsigned int tid = std::stoul(oss.str());

    oss.str("");
    oss.clear();

    oss << "0x" << std::setw(8) << std::setfill('0')
        << std::hex << tid;

    return oss.str();
}

std::string& LogLiteImpl::getModuleTag(void)
{
    return mLogConfig.mod_tag;
}

std::string& LogLiteImpl::getLogLevel(void)
{
    return mLogLevel;
}

std::string& LogLiteImpl::getLogTag(void)
{
    return mLogConfig.log_tag;;
}

void LogLiteImpl::reset(void)
{
    mOfs.reset();
    mLogSize = 0;
}

bool LogLiteImpl::reopen()
{
    mOfs.reset(new std::ofstream(mLogConfig.file_path, std::ios::out | std::ios::app));
    if (!mOfs->is_open())
    {
        std::cerr << "call std::ofstream() failed, "
            << "log file path:[" << mLogConfig.file_path << "]" << std::endl;
        return false;
    }

    return true;
}

void LogLiteImpl::rollback(void)
{
    std::map<std::string, std::string> rename_mapped =
    {
        {mLogConfig.file_path, mLogConfig.file_path + ".1"}
    };

    for (size_t index = 1; index != mLogConfig.file_backup; index++)
    {
        std::ostringstream oss;
        oss << mLogConfig.file_path << "." << index;

        std::string log_name = oss.str();
        if (FileHelper::fileExist(log_name))
        {
            oss.str("");
            oss << mLogConfig.file_path << "." << index + 1;
            std::string log_rename = oss.str();

            rename_mapped.insert(std::make_pair(log_name, log_rename));
        }
        else
            break;
    }

    for (auto rit = rename_mapped.crbegin();
        rit != rename_mapped.crend(); rit++)
    {
        if (!FileHelper::moveFile(rit->first, rit->second))
        {
            std::cerr << "roll back log file failed" << std::endl;
            continue;
        }
    }

    return;
}

void LogLiteImpl::addLogRawItem(LOG_LEVEL level, const std::string& log)
{
    //  consider cache this part
    std::ostringstream oss;
    oss << "0x" << std::setw(8) << std::setfill('0')
        << std::hex << std::this_thread::get_id();;
    //  consider cache this part

    LogMetaInfoPtr meta_info(std::make_shared<LogMetaInfo>());
    meta_info->level = level;
    meta_info->msg = log;
    meta_info->tid = oss.str();

    {
        std::unique_lock<std::mutex> lk(mRawItemQueueMutex);
        mLogRawItemQueue.push(meta_info);
    }

    mRawItemQueueCond.notify_one();
}

void LogLiteImpl::addLogFormatItem(std::shared_ptr<std::string> format_log)
{
    std::unique_lock<std::mutex> lk(mFormatItemQueueMutex);
    mLogFormatItemQueue.push(format_log);

    if (mLogFormatItemQueue.size() > LOG_ITEM_BUFF_SIZE)
        mFormatItemQueueCond.notify_one();
}

void LogLiteImpl::processLogRawItem()
{
    {
        std::unique_lock<std::mutex> lk(mMutexCount);
        mInitThreads++;
    }

    while (true)
    {
        std::unique_lock<std::mutex> lk(mRawItemQueueMutex);
        while (mLogRawItemQueue.empty())
        {
            if (!mIsRunning)
            {
                mFormatItemQueueCond.notify_one();
                return;
            }

            mRawItemQueueCond.wait(lk);

#if 0
            if (!mIsRunning && !mLogRawItemQueue.empty())
                return;
#endif
        }

        std::shared_ptr<LogMetaInfo> rawItem = mLogRawItemQueue.front();
        mLogRawItemQueue.pop();
        lk.unlock();

        // process
        std::ostringstream oss;
        std::string log_prefix = "";
        {
            std::unique_lock<std::mutex> lk(mMutex);
            if (rawItem->level < mLogConfig.level)
                continue;

            log_prefix = generatePrefix(rawItem->tid, LogHelper::levelToString(rawItem->level));
        }
        oss << log_prefix << rawItem->msg;

        std::shared_ptr<std::string> format_log(std::make_shared<std::string>(oss.str()));
        addLogFormatItem(format_log);
    }

}

void LogLiteImpl::processLogFormatItem()
{
    {
        std::unique_lock<std::mutex> lk(mMutexCount);
        mInitThreads++;
    }

    while (true)
    {
        std::unique_lock<std::mutex> lk(mFormatItemQueueMutex);
        while (mLogFormatItemQueue.empty())
        {
            if (!mIsRunning)
                return;
            mFormatItemQueueCond.wait_for(lk, std::chrono::seconds(IO_THREAD_INTERVAL_SECOND));
        }

        std::queue<std::shared_ptr<std::string>> format_item_queue;
        format_item_queue.swap(mLogFormatItemQueue);
        lk.unlock();

        // process
#if 0
        for (auto log : format_item_queue)
            logs += *log;
#else
        std::ostringstream oss;
        while (!format_item_queue.empty())
        {
            std::shared_ptr<std::string> log = format_item_queue.front();
            //logs += *log;
            oss << *log << std::endl;
            format_item_queue.pop();
        }
        std::string logs = oss.str();
#endif

        {
            std::unique_lock<std::mutex> lk(mMutex);
            if (mLogSize + logs.size() >= mLogConfig.file_size)
            {
                // close
                reset();

                rollback();

                if (!reopen())
                    return;
            }

            *mOfs << logs;
            mLogSize += logs.size();
        }
    }

}

// ---------------------------------------- //
// LogLite
// ---------------------------------------- //

LogLite::LogLite(const LogConfig& conf)
{
    mImpl.reset(new LogLiteImpl(conf));
}

bool LogLite::init(void)
{
    return mImpl->init();
}

void LogLite::setConfig(const LogConfig& conf)
{
    return mImpl->setConfig(conf);
}

LogConfig LogLite::getConfig(void) const
{
    return mImpl->getConfig();
}

void LogLite::writeLog(LOG_LEVEL level, const std::string& log)
{
    return mImpl->writeLog(level, log);
}

LOG_LITE_NS_END
