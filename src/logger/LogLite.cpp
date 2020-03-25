#include <logger/LogLite.h>
#include <time.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iostream>
#include <map>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <thread>
#include <iterator>

#include <logger/FileHelper.h>

LOG_LITE_NS_BEIGN

// ---------------------------------------- //
// LogLiteImpl
// ---------------------------------------- //

class LogLiteImpl
{
public:
    explicit LogLiteImpl(const LogConfig& conf)
        : mLogConfig(conf)
        , mOfs(NULL)
        , mLogLevel(LogHelper::levelToString(mLogConfig.level))
    {}

    virtual ~LogLiteImpl()
    {
        if (mOfs)
            mOfs->close();
    }

    bool init(void);

    void setConfig(const LogConfig& conf);
    LogConfig getConfig(void) const;

    void writeLog(LOG_LEVEL level, const std::string& log);

protected:
    virtual inline std::string generatePrefix(void);

    virtual inline std::string generateTimestamp(void);
    virtual inline std::string generateThreadID(void);
    virtual inline std::string& getModuleTag(void);
    virtual inline std::string& getLogLevel(void);
    virtual inline std::string& getLogTag(void);

    void reset(void);

    void rollback(void);

private:
    std::mutex mMutex;
    LogConfig mLogConfig;
    std::shared_ptr<std::ofstream> mOfs;
    std::string mLogLevel;
    size_t mLogSize;
};

bool LogLiteImpl::init(void)
{
    FileHelper::getFileSize(mLogConfig.file_path, mLogSize);

    mOfs.reset(new std::ofstream(mLogConfig.file_path, std::ios::out | std::ios::app));
    if (!mOfs->is_open())
    {
        std::cerr << "call std::ofstream() failed, "
            << "log file path:[" << mLogConfig.file_path << "]" << std::endl;
        return false;
    }

    return true;
}

void LogLiteImpl::setConfig(const LogConfig& conf)
{
    mLogConfig = conf;
    mLogLevel = LogHelper::levelToString(mLogConfig.level);
}

LogConfig LogLiteImpl::getConfig(void) const
{
    return mLogConfig;
}

void LogLiteImpl::writeLog(LOG_LEVEL level, const std::string& log)
{
    if (level < mLogConfig.level)
        return;

    if (mLogSize >= mLogConfig.file_size)
    {
        // close
        reset();

        rollback();

        // reopen
        mOfs = std::make_shared<std::ofstream>(mLogConfig.file_path, std::ios::out | std::ios::app);
        if (!mOfs->is_open())
        {
            std::cerr << "call std::ofstream() failed, "
                << "log file path:[" << mLogConfig.file_path << "]" << std::endl;
            return;
        }
    }

    if (mOfs == NULL)
    {
        std::cerr << "logger not be inited, call init() and try again" << std::endl;
        return;
    }

    std::ostringstream oss;
    std::string log_prefix = generatePrefix();
    oss << log_prefix << log << std::endl;

    std::lock_guard<std::mutex> lk(mMutex);
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
    if (mOfs)
    {
        mOfs->close();
        mOfs.reset();
    }

    mLogSize = 0;
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

#if 1
    for (auto rit = rename_mapped.crbegin();
        rit != rename_mapped.crend(); rit++)
#else
    for (std::map<std::string, std::string>::const_reverse_iterator rit = rename_mapped.rbegin();
        rit != rename_mapped.rend(); rit++)
#endif
    {
        if (!FileHelper::moveFile(rit->first, rit->second))
        {
            std::cerr << "roll back log file failed" << std::endl;
            continue;
        }
    }

    return;
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
