#ifndef __COMMON_BASE_I_BASE_SERVICE_H__
#define __COMMON_BASE_I_BASE_SERVICE_H__
#include <common/base/common.h>
#include <common/base/IBaseObject.h>
#include <common/base/cbase_export.h>

CBASE_BEGIN

class CBASE_EXPORT IBaseService : public IBaseObject
{
public:
    IBaseService(const char *tag)
        : IBaseObject(tag)
    {

    }
    virtual ~IBaseService()
    {
    }
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isStarted() const = 0;

protected:

};

typedef boost::intrusive_ptr<IBaseService> IBaseServicePtr;

CBASE_END

#endif
