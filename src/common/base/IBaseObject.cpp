#include <common/base/IBaseObject.h>
#include <boost/format.hpp>
#include <common/base/logger/LoggerManager.h>

using namespace CBASE_LOGGER_NAMESPACE;

CBASE_BEGIN

IBaseObject::IBaseObject()
    : mTag("")
    , mLogger(LoggerManager::getLogger("SysLog"))
{

}

IBaseObject::IBaseObject(const char *tag)
    : mTag(tag)
    , mLogger(LoggerManager::getLogger("SysLog", mTag))
{

}

IBaseObject::~IBaseObject()
{

}

std::string IBaseObject::toString() const
{
    return (boost::format("IBaseObject( tag: %s)") % mTag).str() ;
}

bool IBaseObject::setTag(const char *tag)
{
    mTag = tag;
    if (tag)
        mLogger = LoggerManager::getLogger("SysLog", tag);
    else
        mLogger = LoggerManager::getLogger("SysLog");
    return true;
}

const char * IBaseObject::getTag() const
{
    return mTag;
}


std::ostream &operator<<(std::ostream &os, const IBaseObject &obj)
{
    return os << obj.toString();
}

CBASE_END
