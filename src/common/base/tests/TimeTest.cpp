#include <iostream>
#include <memory>
#include <mutex>
#include <common/base/common.h>
#include <common/base/Time.h>

using namespace std;
using namespace CBASE_NAMESPACE;

int main(int argc, const char* argv[])
{
	int sleep_sec = 5;
	std::cout << "timer start " << std::endl;
	std::cout << "start to sleep:" << sleep_sec << " s" << std::endl;

	Time::HighPrecisionTimeType t1 = Time::rdtsc();
#ifdef WIN32
	::Sleep(sleep_sec * 1000);
#else
	::sleep(sleep_sec);
#endif
	Time::HighPrecisionTimeType t2 = Time::rdtsc();
	std::cout << "sleep done" << std::endl;

	std::cout << "time diff is " << Time::diff(t1, t2) << " s" << std::endl;
	std::cout << "time diff is " << Time::diffMs(t1, t2) << " ms" << std::endl;
	std::cout << "time diff is " << Time::diffUs(t1, t2) << " us" << std::endl;

    // -------------------- //

    TimeElapse te;

	std::mutex mtx;
	int count = 0;
    int lock_times = 1000000;
    std::cout << "start lock mutex " << lock_times << " times" << std::endl;
	for (int i = 0; i < lock_times; i++)
	{
		std::unique_lock<std::mutex> lk(mtx);
		count += 1;
		lk.unlock();
	}

    te.stop();
    std::cout << "lock mutex " << lock_times << " times done" << std::endl;

    std::cout << "lock times:" << lock_times << ", cost time:" << te.getElapsedSeconds() << " s" << std::endl;
    std::cout << "lock times:" << lock_times << ", cost time:" << te.getElapsedMilliseconds() << " ms" << std::endl;
    std::cout << "lock times:" << lock_times << ", cost time:" << te.getElapsedMicroseconds() << " us" << std::endl;

	return 0;
}
