#ifndef __COMMON_BASE_I_BASE_OBJECT_H__
#define __COMMON_BASE_I_BASE_OBJECT_H__
#include <boost/intrusive_ptr.hpp>
#include <log4cxx/logger.h>
#include <common/base/common.h>
#include <common/base/RefCount.h>

CBASE_BEGIN

class IBaseObject : virtual public RefCount
{
public:
    IBaseObject();
    IBaseObject(const char *tag);
    virtual ~IBaseObject();
    virtual std::string toString() const;

    bool setTag(const char *tag);
    const char *getTag() const;

    friend std::ostream & operator<<(std::ostream &os, const IBaseObject &obj);
protected:
    const char *mTag;
    log4cxx::LoggerPtr mLogger;
};

typedef boost::intrusive_ptr<IBaseObject> IBaseObjectPtr;

CBASE_END
#endif
