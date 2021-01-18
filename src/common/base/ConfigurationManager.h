#ifndef __COMMON_BASE_CONFIGURATION_MANAGER_H__
#define __COMMON_BASE_CONFIGURATION_MANAGER_H__

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <common/base/common.h>
#include <common/base/ConfigParser.h>
#include <common/base/IBaseService.h>
#include <common/base/cbase_export.h>

using boost::property_tree::basic_ptree;

CBASE_BEGIN

struct CBASE_EXPORT CMParameter
{
    CMParameter(const std::string &filename="");
    std::string mFileName;
};

class CBASE_EXPORT ConfigurationManager : public IBaseService
{
public:
    ConfigurationManager(const std::string &filename);
    ConfigurationManager(const CMParameter &param);
    ConfigurationManager();

    bool start();
    void stop();
    bool isStarted() const;

    const wptree & get(const std::string &path) const
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        return mParser->get(path);
    }

    const wptree & get(const std::wstring &path) const
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        return mParser->get(path);
    }

    template <typename T>
        T getValue(const std::string &path) const
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        return mParser->getValue<T>(path);
    }

    template <typename T>
        T getValue(const std::string &path, const T &defaultValue)
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        return mParser->getValue<T>(path, defaultValue);
    }

    template <typename T>
        bool setValue(const std::string &path, const T& value)
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        bool result = mParser->setValue(path, value);
        if (result)
            mParser->save();
        rl.unlock();

        return result;
    }
    template <typename T>
        T getValue(const std::wstring &path) const
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        return mParser->getValue<T>(path);
    }

    template <typename T>
        T getValue(const std::wstring &path, const T &defaultValue)
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        return mParser->getValue<T>(path, defaultValue);
    }

    template <typename T>
        bool setValue(const std::wstring &path, const T& value)
    {
        read_lock rl(mLock);
        BOOST_ASSERT(mParser);
        bool result = mParser->setValue(path, value);
        mParser->save();
        rl.unlock();

        return result;
    }

    bool setParameter(const CMParameter &param);

    const CMParameter &getParameter() const;


private:
    mutable rwmutex mLock;
    ConfigParserPtr mParser;
    CMParameter mParam;
};

typedef boost::intrusive_ptr<ConfigurationManager> ConfigurationManagerPtr;

CBASE_END

#endif
