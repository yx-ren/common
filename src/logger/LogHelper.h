#ifndef __LOG_LITE_LOG_HELPER_H__
#define __LOG_LITE_LOG_HELPER_H__

#include <string>
#include <map>
#include <logger/common.h>
#include <logger/LogConfig.h>

LOG_LITE_NS_BEIGN

class LogHelper
{
public:
    static std::string& levelToString(LOG_LEVEL level);

    static std::string modeToString(int mode);

    static std::string configToString(LogConfig conf);

private:
    static std::map<LOG_LEVEL, std::string> mLevelString;
};

LOG_LITE_NS_END

#endif
