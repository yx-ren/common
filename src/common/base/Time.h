#ifndef __COMMON_BASE_LOGGER_TIME_H__
#define __COMMON_BASE_LOGGER_TIME_H__

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <common/base/common.h>
#include <common/base/logger/Logger.h>
#include <common/base/logger/LoggerManager.h>

#if 0
#else // 0
#if defined(__linux__)
#include <time.h>
#elif  defined(__APPLE__) && defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#elif defined(WIN32)
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#else
#error "no high resolution available."
#endif

#endif // 0

CBASE_BEGIN

class CBASE_EXPORT Time
{
public:
    typedef uint64_t HighPrecisionTimeType;
    static const uint64_t NS_PER_SEC = 1000000000UL;
    static const uint64_t NS_PER_MS = 1000000UL;
    static const uint64_t NS_PER_US = 1000UL;
#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#if defined(__APPLE__) && defined(__MACH__)
    static inline HighPrecisionTimeType rdtsc(void)
    {
        clock_serv_t cclock;
        mach_timespec_t ts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &ts);
        mach_port_deallocate(mach_task_self(), cclock);
        return uint64_t(ts.tv_sec) * NS_PER_SEC + ts.tv_nsec;
    }
#else
    static inline HighPrecisionTimeType rdtsc(void)
    {
        struct timespec ts = {0, 0};
        if (::clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        {
            CB_LOG_SYS_ERROR("failed to get high resolution timestamp.");
        }
        return uint64_t(ts.tv_sec) * NS_PER_SEC + ts.tv_nsec;
    }
#endif
    static inline int64_t diff(const HighPrecisionTimeType &t1, const HighPrecisionTimeType &t2)
    {
        return (t2 - t1) / NS_PER_SEC;
    }

    static inline int64_t diffMs(const HighPrecisionTimeType &t1, const HighPrecisionTimeType &t2)
    {
        return (t2 - t1) / NS_PER_MS;
    }

    static inline int64_t diffUs(const HighPrecisionTimeType &t1, const HighPrecisionTimeType &t2)
    {
        return (t2 - t1) / NS_PER_US;
    }
#elif defined(WIN32)
	static uint64_t getPerformanceFrequency()
	{
		LARGE_INTEGER freq = { 0, 0 };
		if (!QueryPerformanceFrequency(&freq))
		{
		    CB_LOG_SYS_ERROR("failed to get high resolution timestamp.");
		}
		return freq.QuadPart;
	}

	static uint64_t sPerformanceFrequency; 

    static inline HighPrecisionTimeType rdtsc(void)
    {
		LARGE_INTEGER ts = { 0, 0 };
        if (!QueryPerformanceCounter(&ts))
        {
            CB_LOG_SYS_ERROR("failed to get high resolution timestamp.");
        }
        return ts.QuadPart;
    }

    static inline int64_t diff(const HighPrecisionTimeType &t1, const HighPrecisionTimeType &t2)
    {
        BOOST_ASSERT(sPerformanceFrequency!=0);
        return (t2 - t1) / sPerformanceFrequency ;
    }

    static inline int64_t diffMs(const HighPrecisionTimeType &t1, const HighPrecisionTimeType &t2)
    {
        BOOST_ASSERT(sPerformanceFrequency!=0);
        return (t2 - t1)*(NS_PER_SEC/NS_PER_MS) / sPerformanceFrequency ;
    }

    static inline int64_t diffUs(const HighPrecisionTimeType &t1, const HighPrecisionTimeType &t2)
    {
        BOOST_ASSERT(sPerformanceFrequency!=0);
        return (t2 - t1)*(NS_PER_SEC/NS_PER_US) / sPerformanceFrequency ;
    }
    
#else
#error "no high resolution timer available."
#endif 
#if 0
    typedef uint64_t HighPrecisionTimeType;


    static inline uint64_t rdtsc(void)
    {
#if defined(__i386__)
        uint64_t x;
        __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
        return x;
#elif defined(__x86_64__)
        unsigned hi, lo;
        __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
        return ( (uint64_t)lo)|( ((uint64_t)hi)<<32 );
#elif defined(WIN32)
        return __rdtsc();
#else
#error "No rdtsc available!"
#endif
    }
#endif // 0    
    typedef boost::posix_time::ptime TimeType;
    
    static inline TimeType currentTime()
    {
        return boost::posix_time::microsec_clock::local_time();
    }

    static inline uint32_t currentTimeInSecond()
    {
        TimeType epoch(boost::gregorian::date(1970, 1, 1));
        return (currentTime() - epoch).total_seconds();
    }

    static inline TimeType afterMs(uint32_t ms)
    {
        return currentTime() + boost::posix_time::milliseconds(ms);
    }

    static inline TimeType beforeMs(uint32_t ms)
    {
        return currentTime() - boost::posix_time::milliseconds(ms);
    }

    static inline int64_t diff(const TimeType &t1, const TimeType &t2)
    {
        return (t2 - t1).total_seconds();
    }
    
    static inline int64_t diffMs(const TimeType &t1, const TimeType &t2)
    {
        return (t2 - t1).total_milliseconds();
    }

    static inline int64_t diffUs(const TimeType &t1, const TimeType &t2)
    {
        return (t2 - t1).total_microseconds();
    }

    static inline bool expired(const TimeType &t)
    {
        return currentTime() >= t;
    }

    static inline bool expired(const TimeType &t1, const TimeType &t2)
    {
        return t2 >= t1;
    }
    
};


class TimeElapse
{
public:
    TimeElapse()
    {
        mStart = mStop = Time::rdtsc();
    }
    void start()
    {
        mStart = Time::rdtsc();
    }
    uint64_t stop()
    {
        mStop = Time::rdtsc();
        return Time::diffUs(mStart, mStop);
    }

    uint64_t getElapsedSeconds()
    {
        return Time::diff(mStart, mStop);
    }

    uint64_t getElapsedMilliseconds()
    {
        return Time::diffMs(mStart, mStop);
    }

    uint64_t getElapsedMicroseconds()
    {
        return Time::diffUs(mStart, mStop);
    }
private:
    Time::HighPrecisionTimeType mStart;
    Time::HighPrecisionTimeType mStop;
};

class Timer
{
public:
    Timer() { mStart = Time::currentTime(); }

    void restart()
    {
        mStart = Time::currentTime();
    }

    uint64_t elapsed()
    {
        return Time::diff(mStart, Time::currentTime());
    }

    uint64_t elapsedMicroseconds()
    {
        return Time::diffUs(mStart, Time::currentTime());
    }
    uint64_t elapsedMilliseconds()
    {
        return Time::diffMs(mStart, Time::currentTime());
    }

private:
    Time::TimeType mStart;
};
CBASE_END

#endif // SKYGUARD_BASE_TIME_H_H
