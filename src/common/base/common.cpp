#include <common/base/common.h>
//#include <boost/backtrace.hpp>
#include <boost/format.hpp>
#include <common/base/logger/LoggerManager.h>

CBASE_BEGIN
write_lock::write_lock(boost::shared_mutex &m)
{
    try
    {
        boost::unique_lock<boost::shared_mutex> swap_lock(m);
        m_internal_write_lock.swap(swap_lock);
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("write_lock failure: " << bt.trace());
#endif
        throw;
    }
}
write_lock::write_lock(boost::shared_mutex &m, boost::defer_lock_t dl)
{
    try
    {
        boost::unique_lock<boost::shared_mutex> swap_lock(m, dl);
        m_internal_write_lock.swap(swap_lock);
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("write_lock failure: " << bt.trace());
#endif
        throw;
    }
}
write_lock::write_lock(boost::shared_mutex &m, boost::adopt_lock_t al)
{
    try
    {
        boost::unique_lock<boost::shared_mutex> swap_lock(m, al);
        m_internal_write_lock.swap(swap_lock);
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("write_lock failure: " << bt.trace());
#endif
        throw;
    }
}
write_lock::write_lock(boost::shared_mutex &m, boost::try_to_lock_t tl)
{
    try
    {
        boost::unique_lock<boost::shared_mutex> swap_lock(m, tl);
        m_internal_write_lock.swap(swap_lock);
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("write_lock failure: " << bt.trace());
#endif
        throw;
    }
}

void write_lock::lock()
{
    try
    {
        m_internal_write_lock.lock();
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("write_lock failure: " << bt.trace());
#endif
        throw;
    }
}

bool write_lock::try_lock()
{
    try
    {
        return m_internal_write_lock.try_lock();
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("write_lock failure: " << bt.trace());
#endif
        throw;
    }
}

void write_lock::unlock()
{
    try
    {
        m_internal_write_lock.unlock();
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("write_lock failure: " << bt.trace());
#endif
        throw;
    }
}

read_lock::read_lock(boost::shared_mutex &m)
{
    try
    {
        boost::shared_lock<boost::shared_mutex> swap_lock(m);
        m_internal_read_lock.swap(swap_lock);
    }
    catch (...)
    {
#if 0
        //boost::backtrace bt;
        //CB_ERROR("read_lock failure: " << bt.trace());
#endif
        throw;
    }
}
read_lock::read_lock(boost::shared_mutex &m, boost::defer_lock_t dl)
{
    try
    {
        boost::shared_lock<boost::shared_mutex> swap_lock(m, dl);
        m_internal_read_lock.swap(swap_lock);
    }
    catch (...)
    {
        //boost::backtrace bt;
        //CB_ERROR("read_lock failure: " << bt.trace());
        throw;
    }
}
read_lock::read_lock(boost::shared_mutex &m, boost::adopt_lock_t al)
{
    try
    {
        boost::shared_lock<boost::shared_mutex> swap_lock(m, al);
        m_internal_read_lock.swap(swap_lock);
    }
    catch (...)
    {
        //boost::backtrace bt;
        //CB_ERROR("read_lock failure: " << bt.trace());
        throw;
    }
}
read_lock::read_lock(boost::shared_mutex &m, boost::try_to_lock_t tl)
{
    try
    {
        boost::shared_lock<boost::shared_mutex> swap_lock(m, tl);
        m_internal_read_lock.swap(swap_lock);
    }
    catch (...)
    {
        //boost::backtrace bt;
        //CB_ERROR("read_lock failure: " << bt.trace());
        throw;
    }
}

void read_lock::lock()
{
    try
    {
        m_internal_read_lock.lock();
    }
    catch (...)
    {
        //boost::backtrace bt;
        //CB_ERROR("read_lock failure: " << bt.trace());
        throw;
    }
}

bool read_lock::try_lock()
{
    try
    {
        return m_internal_read_lock.try_lock();
    }
    catch (...)
    {
        //boost::backtrace bt;
        //CB_ERROR("read_lock failure: " << bt.trace());
        throw;
    }
}

void read_lock::unlock()
{
    try
    {
        m_internal_read_lock.unlock();
    }
    catch (...)
    {
        //boost::backtrace bt;
        //CB_ERROR("read_lock failure: " << bt.trace());
        throw;
    }
}

CBASE_END
