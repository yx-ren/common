#include <common/base/Time.h>

CBASE_BEGIN

#ifdef WIN32
uint64_t Time::sPerformanceFrequency = Time::getPerformanceFrequency();
#endif

CBASE_END
