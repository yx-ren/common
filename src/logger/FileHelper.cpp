#include <logger/FileHelper.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
LOG_LITE_NS_BEIGN

bool FileHelper::getFileSize(const std::string& path, size_t& fsize)
{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs.is_open())
        return false;

    std::string file_buf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    fsize = file_buf.size();

    return true;
}

bool FileHelper::fileExist(const std::string& path)
{
    std::ifstream ifs(path.c_str());
    if (ifs.is_open())
    {
        ifs.close();
        return true;
    }

    return false;
}

bool FileHelper::moveFile(const std::string& oldPath, const std::string& newPath)
{
    int ret = ::rename(oldPath.c_str(), newPath.c_str());
    if (ret != 0)
    {
        std::cerr << "call rename() failed, error:[" << errno << "], "
            << "old path:[" << oldPath << "], "
            << "new path:[" << newPath << "]" << std::endl;
        return false;
    }

    return true;
}

bool FileHelper::removeFile(const std::string& path)
{
    int ret = ::remove(path.c_str());
    if (ret != 0)
    {
        std::cerr << "call rename() failed, error:[" << errno << "], "
            << "path:[" << path << "]" << std::endl;
        return false;
    }

    return true;
}

LOG_LITE_NS_END
