#ifndef __COMMON_BASE_COMMON_H__
#define __COMMON_BASE_COMMON_H__

#include <memory>
#include <atomic>
#include <list>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <boost/current_function.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/pointer_cast.hpp>

#define CBASE_NAMESPACE __common__::base

#define CBASE_BEGIN namespace __common__ { namespace base {
#define CBASE_END } /*end of namespace __common__ */ } /*end of namespace base */

#define CB_TRACE_FUNC() CB_TRACE(BOOST_CURRENT_FUNCTION)

CBASE_BEGIN
template <typename _Tp>
    using sp = boost::shared_ptr<_Tp>;

template <typename T, typename U>
    sp<T> static_pointer_cast(const sp<U> &p)
{
    return boost::static_pointer_cast<T, U>(p);
}

template <typename T, typename U>
    sp<T> dynamic_pointer_cast(const sp<U> &p)
{
    return boost::dynamic_pointer_cast<T, U>(p);
}

template <typename T, typename U>
    sp<T> const_pointer_cast(const sp<U> &p)
{
    return boost::const_pointer_cast<T, U>(p);
}

template <typename T, typename U>
    sp<T> reinterpret_pointer_cast(const sp<U> &p)
{
    return boost::reinterpret_pointer_cast<T, U>(p);
}

template <typename _Tp>
    using wp = boost::weak_ptr<_Tp>;

template <typename _Tp>
    using up = boost::scoped_ptr<_Tp>;

typedef boost::shared_mutex rwmutex;

class write_lock
{
public:
    write_lock()
    {
    }
    write_lock(boost::shared_mutex &m);

    write_lock(boost::shared_mutex &m, boost::defer_lock_t dl);

    write_lock(boost::shared_mutex &m, boost::adopt_lock_t al);

    write_lock(boost::shared_mutex &m, boost::try_to_lock_t tl);

    void lock();

    bool try_lock();

    void unlock();
private:
    boost::unique_lock<boost::shared_mutex> m_internal_write_lock;
};

class read_lock
{
public:
    read_lock()
    {
    }
    read_lock(boost::shared_mutex &m);

    read_lock(boost::shared_mutex &m, boost::defer_lock_t dl);

    read_lock(boost::shared_mutex &m, boost::adopt_lock_t al);

    read_lock(boost::shared_mutex &m, boost::try_to_lock_t tl);

    void lock();

    bool try_lock();

    void unlock();
private:
    boost::shared_lock<boost::shared_mutex> m_internal_read_lock;
};

CBASE_END

#endif
