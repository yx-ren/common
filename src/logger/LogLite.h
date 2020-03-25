#ifndef __LOG_LITE_LOG_LITE_H__
#define __LOG_LITE_LOG_LITE_H__

#include <string>
#include <memory>
#include <logger/common.h>
#include <logger/LogConfig.h>
#include <logger/LogHelper.h>

LOG_LITE_NS_BEIGN

class LogLiteImpl;
class LogLite
{
public:
    explicit LogLite(const LogConfig& conf);
    LogLite(const LogLite&) = delete;
    LogLite& operator=(const LogLite&) = delete;

    bool init(void);

    void setConfig(const LogConfig& conf);
    LogConfig getConfig(void) const;

    void writeLog(LOG_LEVEL level, const std::string& log);

private:
    std::shared_ptr<LogLiteImpl> mImpl;
};

LOG_LITE_NS_END

#endif
