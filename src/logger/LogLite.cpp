#include <logger/LogLite.h>
#include <time.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <thread>
#include <iterator>

LOG_LITE_NS_BEIGN

class FileHelper
{
public:
    FileHelper() = delete;
    FileHelper(const FileHelper&) = delete;
    FileHelper& operator=(const FileHelper&) = delete;

    static bool getFileSize(std::string& path, size_t& fsize);
    static bool getFileName(std::string& path);
    static bool getFilePathPrefix(std::string& path);
    static bool moveFile(std::string& oldPath, std::string& newPath);
    static bool removeFile(std::string& path);
};

bool FileHelper::getFileSize(std::string& path, size_t& fsize)
{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
        return false;

    std::string file_buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    fsize = file_buf.size();

    return true;
}

bool FileHelper::getFileName(std::string& path)
{
    return true;
}

bool FileHelper::getFilePathPrefix(std::string& path)
{
    return true;
}

bool FileHelper::moveFile(std::string& oldPath, std::string& newPath)
{
    return true;
}

bool FileHelper::removeFile(std::string& path)
{
    return true;
}

// ---------------------------------------- //
// LogLiteImpl
// ---------------------------------------- //

class LogLiteImpl
{
public:
    explicit LogLiteImpl(const LogConfig& conf)
        : mLogConfig(conf)
        , mOfs(NULL)
    {}

    virtual ~LogLiteImpl()
    {
        if (mOfs)
            mOfs->close();
    }

    bool init(void);

    void setConfig(const LogConfig& conf);
    LogConfig getConfig(void);

    void writeLog(LOG_LEVEL level, const std::string& log);
    void writeLog(LOG_LEVEL level, const std::string& log, const std::string& tag);

protected:
    virtual inline std::string generatePrefix(void);

    virtual inline std::string generateTimestamp(void);
    virtual inline std::string generateThreadID(void);
    virtual inline std::string generateModuleTag(void);
    virtual inline std::string generateLogLevel(void);
    virtual inline std::string generateLogTag(void);
    virtual inline std::string generateClassTag(void);
    virtual inline std::string generateFunctionTag(void);

private:
    std::mutex mMutex;
    LogConfig mLogConfig;
    std::shared_ptr<std::ofstream> mOfs;
};

bool LogLiteImpl::init(void)
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

void LogLiteImpl::setConfig(const LogConfig& conf)
{
    mLogConfig = conf;
}

LogConfig LogLiteImpl::getConfig(void)
{
    return mLogConfig;
}

void LogLiteImpl::writeLog(LOG_LEVEL level, const std::string& log)
{
    if (level < mLogConfig.level)
        return;

    if (mOfs == NULL)
    {
        std::cerr << "logger not be inited, call init() and try again" << std::endl;
        return;
    }

    std::ostringstream oss;
    std::string log_prefix = generatePrefix();
    oss << log_prefix << log << std::endl;

    std::lock_guard<std::mutex> lk(mMutex);
    if (mLogConfig.mode & LOG_MODE_FILE)
    {
        *mOfs << oss.str();
    }

    if (mLogConfig.mode & LOG_MODE_CONSOLE)
    {
        std::cout << oss.str();
    }

    return;
}

void LogLiteImpl::writeLog(LOG_LEVEL level, const std::string& log, const std::string& tag)
{
    return;
}

std::string LogLiteImpl::generatePrefix(void)
{
    std::ostringstream oss;
    oss << generateTimestamp() << " "
        << "[" << generateThreadID() << "] "
        << "<" << generateModuleTag()<< ">: "
        << generateLogLevel()<< " "
        << generateLogTag() << "." << generateFunctionTag() << " - ";

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

std::string LogLiteImpl::generateModuleTag(void)
{
    return mLogConfig.mod_tag;
}

std::string LogLiteImpl::generateLogLevel(void)
{
    return LogHelper::levelToString(mLogConfig.level);
}

std::string LogLiteImpl::generateLogTag(void)
{
    return mLogConfig.log_tag;;
}

std::string LogLiteImpl::generateClassTag(void)
{
    return "";
}

std::string LogLiteImpl::generateFunctionTag(void)
{
    return "";
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

LogConfig LogLite::getConfig(void)
{
    return mImpl->getConfig();
}

void LogLite::writeLog(LOG_LEVEL level, const std::string& log)
{
    return mImpl->writeLog(level, log);
}

void LogLite::writeLog(LOG_LEVEL level, const std::string& log, const std::string& tag)
{
    return mImpl->writeLog(level, log, tag);
}

LOG_LITE_NS_END
