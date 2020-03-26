#ifndef __LOG_LITE_BLOCK_TASK_QUEUE_H__
#define __LOG_LITE_BLOCK_TASK_QUEUE_H__
#include <string>
#include <iostream>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <logger/common.h>
#ifndef WIN32
#include <condition_variable>
#endif

LOG_LITE_NS_BEIGN
#define MAX_TASKS_NUMBERS 1000
#define CONSUMER_THREAD_NUMBERS 1

template<typename T>
class BlockTaskQueue
{
public:
    typedef std::function<bool(T&)> processTaskCallback;
    typedef std::function<bool()> notifyCallback;

    BlockTaskQueue(size_t maxTaskCount = MAX_TASKS_NUMBERS,
            size_t consumerThreadNumbers = CONSUMER_THREAD_NUMBERS);

    virtual ~BlockTaskQueue();

public:
    virtual bool addTask(const T& task);

    virtual void registerProcessCallback(processTaskCallback cb);

    virtual void registerNotifyCallback(notifyCallback cb);

    virtual void setCondWaitTimeout(int ms);

    virtual void notify();

    virtual void size() const;

    virtual void enableEvent();

    virtual void disableEvent();

protected:
    virtual bool processTask(void);

protected:
    BlockTaskQueue(const BlockTaskQueue&) = delete;
    BlockTaskQueue& operator=(const BlockTaskQueue&) = delete;

protected:
    size_t mMaxTaskCount;
    std::queue<T> mTaskQueue;
    std::mutex mMutex;
    std::condition_variable mCond;
    std::mutex mMutexCB;
    processTaskCallback mProcessTaskCB;
    notifyCallback mNotifyCB;
    std::vector<std::shared_ptr<std::thread>> mThreads;
    int mCondTimeoutms;
    bool mIsEventEnable;
};

// -------------------- //
// BlockTaskQueue impl
// -------------------- //

template<typename T>
BlockTaskQueue<T>::BlockTaskQueue(size_t maxTaskCount = MAX_TASKS_NUMBERS,
        size_t consumerThreadNumbers = CONSUMER_THREAD_NUMBERS)
    : mMaxTaskCount(maxTaskCount)
    , mProcessTaskCB(NULL)
    , mNotifyCB(NULL)
    , mCondTimeoutms(-1)
    , mIsEventEnable(true)
{
    for (int i = 0; i != CONSUMER_THREAD_NUMBERS; i++)
    {
        auto th = std::make_shared<std::thread>(std::bind(BlockTaskQueue::processTask, this));
        mThreads.push_back(th);
    }
}

template<typename T>
BlockTaskQueue<T>::~BlockTaskQueue()
{}

template<typename T>
bool BlockTaskQueue<T>::addTask(const T& task)
{
    std::unique_lock<std::mutex> lk(mMutex);

    if (mTaskQueue.size() == mMaxTaskCount)
    {
        std::cerr << "task queue is full" << std::endl;
        return false;
    }

    mTaskQueue.push(task);

    if (mIsEventEnable)
    {
        if (mNotifyCB == NULL || (mNotifyCB && mNotifyCB()))
            mCond.notify_all();
    }

    return true;
}

template<typename T>
void BlockTaskQueue<T>::registerProcessCallback(processTaskCallback cb)
{
    std::unique_lock<std::mutex> lk(mMutexCB);
    mProcessTaskCB = cb;
}

template<typename T>
void BlockTaskQueue<T>::registerNotifyCallback(notifyCallback cb)
{
    std::unique_lock<std::mutex> lk(mMutexCB);
    mNotifyCB = cb;
}

template<typename T>
virtual void BlockTaskQueue<T>::setCondWaitTimeout(int ms)
{
    std::unique_lock<std::mutex> lk(mMutexCB);
    mCondTimeoutms = ms;
}

template<typename T>
virtual void BlockTaskQueue<T>::notify()
{
    std::unique_lock<std::mutex> lk(mMutex);
    mCond.notify_all();
}

template<typename T>
virtual void BlockTaskQueue<T>::size() const
{
    std::unique_lock<std::mutex> lk(mMutex);
    return mTaskQueue.size();
}

template<typename T>
virtual void BlockTaskQueue<T>::enableEvent()
{
    std::unique_lock<std::mutex> lk(mMutex);
    mIsEventEnable = true;
}

template<typename T>
virtual void BlockTaskQueue<T>::disableEvent()
{
    std::unique_lock<std::mutex> lk(mMutex);
    mIsEventEnable = false;
}

template<typename T>
bool BlockTaskQueue<T>::processTask(void)
{
    while (true)
    {
#if 1
        // once lock process one task? swap queue may be fast?
        // use swap maybe cause one thread consume task by serial, not parallel
        T task;
        {
            std::unique_lock<std::mutex> lk(mMutex);
            while (mTaskQueue.empty())
                mCond.wait_for(lk, mCondTimeoutms < 0 ? std::chrono::hours(1) : std::chrono::milliseconds(mCondTimeoutms));

            task = mTaskQueue.front();
            mTaskQueue.pop();
        }

        {
            std::unique_lock<std::mutex> lk(mMutexCB);
            if (mProcessTaskCB && !mProcessTaskCB(task))
            {
                std::cerr << "process task failed" << std::endl;
                return false;
            }
        }
#else
        // use swap
        std::queue<T> task_queue;
        {
            std::unique_lock lk(mMutex);
            while (mTaskQueue.empty())
                mCond.wait(lk);

            mTaskQueue.swap(task_queue);
        }

        {
            std::unique_lock<std::mutex> lk(mMutexCB);
            for (auto& task : task_queue)
            {
                if (!mProcessTaskCB(task))
                {
                    std::cerr << "process task failed" << std::endl;
                    return false;
                }
            }
        }

#endif
    }

    return true;
}

LOG_LITE_NS_END

#endif
