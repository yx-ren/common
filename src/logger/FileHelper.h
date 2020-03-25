#ifndef __LOG_LITE_FILE_HELPER_H__
#define __LOG_LITE_FILE_HELPER_H__
#include <string>
#include <logger/common.h>

LOG_LITE_NS_BEIGN

class FileHelper
{
public:
    static bool getFileSize(const std::string& path, size_t& fsize);
    static bool fileExist(const std::string& path);
    static bool moveFile(const std::string& oldPath, const std::string& newPath);
    static bool removeFile(const std::string& path);
};


LOG_LITE_NS_END

#endif
