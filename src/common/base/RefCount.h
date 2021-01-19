#ifndef __COMMON_BASE_REF_COUNT_H__
#define __COMMON_BASE_REF_COUNT_H__
#include <atomic>
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <common/base/common.h>
#include <common/base/cbase_export.h>

CBASE_BEGIN
class RefCount;

class CBASE_EXPORT RefCount : private boost::noncopyable
{
public:
#ifdef WIN32
RefCount()
{
	m_refcount = 0;
}
#else
RefCount() : m_refcount(0)
{
}
#endif
    virtual ~RefCount()
    {

    }
    friend void intrusive_ptr_add_ref(const RefCount *rc);
    friend void intrusive_ptr_release(const RefCount *rc);
private:
    mutable std::atomic_int m_refcount;
};

inline void intrusive_ptr_add_ref(const RefCount *rc)
{
    ++rc->m_refcount;
}

inline void intrusive_ptr_release(const RefCount *rc)
{
    if (--rc->m_refcount == 0)
        delete rc;
}

CBASE_END

#endif
