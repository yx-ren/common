#ifndef __COMMON_BASE_SCOPEDGUARD_H__
#define __COMMON_BASE_SCOPEDGUARD_H__

#include <common/base/common.h>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

CBASE_BEGIN

class ScopedGuard : private boost::noncopyable
{
public:
    explicit ScopedGuard(boost::function<void ()> clearupFunc)
        : mClearupFunc(clearupFunc)
        , mDismissed(false)
    {

    }

    ~ScopedGuard()
    {
        if (!mDismissed)
        {
            mClearupFunc();
        }
    }

    void dismiss()
    {
        mDismissed = true;
    }
private:
    boost::function<void ()> mClearupFunc;
    bool mDismissed;
};

#define SCOPEDGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEDGUARD_LINENAME(name, line) SCOPEDGUARD_LINENAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) ::skyguard::base::ScopedGuard SCOPEDGUARD_LINENAME(EXIT, __LINE__)(callback)

CBASE_END
#endif
