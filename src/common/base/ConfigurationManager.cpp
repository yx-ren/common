#include <common/base/ConfigurationManager.h>
#include <boost/filesystem.hpp>
#include <common/base/logger/LoggerManager.h>

using namespace CBASE_LOGGER_NAMESPACE;

CBASE_BEGIN

CMParameter::CMParameter(const std::string &filename)
    : mFileName(filename)
{

}

ConfigurationManager::ConfigurationManager(const std::string &filename)
    : IBaseService("cm")
    , mParam(filename)
{

}

ConfigurationManager::ConfigurationManager(const CMParameter &param)
    : IBaseService("cm")
    , mParam(param)
{

}

ConfigurationManager::ConfigurationManager()
    : IBaseService("cm")
{

}

bool ConfigurationManager::start()

{
    CB_TRACE_FUNC();

    write_lock wl(mLock);
    if (mParser)
    {
        CB_ERROR("configuration manager has already been started.");
        return false;
    }

    if (!mParser)
    {
        if (!boost::filesystem::exists(mParam.mFileName))
        {
            CB_ERROR("config file '" << mParam.mFileName << "' doesn't exist.");
            return false;
        }
        mParser.reset(new ConfigParser(mParam.mFileName));
        CB_DEBUG("configuartion manager for '" << mParam.mFileName << "' is started.");
    }
    return true;
}

void ConfigurationManager::stop()
{
    CB_TRACE_FUNC();

    write_lock wl(mLock);
    if (mParser)
    {
        mParser.reset();
        CB_DEBUG("configuration manager for '" << mParam.mFileName << "' is stopped.");
    }
}

bool ConfigurationManager::isStarted() const
{
    read_lock rl(mLock);
    return mParser.get() != 0;
}

bool ConfigurationManager::setParameter(const CMParameter &param)
{
    CB_TRACE_FUNC();

    write_lock wl(mLock);
    if (mParser)
    {
        CB_ERROR("cannot set the parameter of configuration manager as the service is started.");
        return false;
    }

    mParam = param;
    return true;
}

const CMParameter &ConfigurationManager::getParameter() const
{
    read_lock rl(mLock);
    return mParam;
}

static std::string wstr2str(const std::wstring &wstr)
{
    return std::string(wstr.begin(), wstr.end());
}

CBASE_END
