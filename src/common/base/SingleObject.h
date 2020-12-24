#ifndef __COMMON_BASE_SINGLE_OBJECT_H__
#define __COMMON_BASE_SINGLE_OBJECT_H__

#include <mutex>
#include <atomic>
#include <common/base/common.h>

CBASE_BEGIN

template<typename T>
class SingleObject
{
public:
    virtual ~SingleObject() {}

    static T* getInstance()
    {
        if (mInstance.load() == NULL)
        {
            std::unique_lock<std::mutex> lk(mMutex);
            if (mInstance.load() == NULL)
                mInstance = new T;
        }

        return mInstance;
    }

    static void freeInstance()
    {
        std::unique_lock<std::mutex> lk(mMutex);
        T* instance = mInstance.exchange(NULL);
        if (instance != NULL)
            delete instance;
    }

protected:
    SingleObject() {}

private:
    static std::mutex mMutex;
    static std::atomic<T*> mInstance;
};

template<typename T>
std::atomic<T*> SingleObject<T>::mInstance(NULL);

template<typename T>
std::mutex SingleObject<T>::mMutex;

CBASE_END

#endif
