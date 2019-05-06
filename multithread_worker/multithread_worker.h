#ifndef MULTITHREAD_WORKER_H
#define MULTITHREAD_WORKER_H

#include <chrono>
#include <queue>
#include <functional>
#include <mutex>


class MultithreadWorkerArguments
{
public:
    virtual bool isValidArgument() const = 0;

    virtual ~MultithreadWorkerArguments();
};


class MultithreadWorker
{
public:
    MultithreadWorker();
    ~MultithreadWorker();

    bool run(int numThreads,
             const std::queue<MultithreadWorkerArguments*>& argumentQueue,
             std::function<void(const MultithreadWorkerArguments*)>& threadFunction);

    void waitUntilTheEnd(const std::chrono::milliseconds& sleepForTime) const;

private:
    void threadWorker(std::function<void(const MultithreadWorkerArguments*)>& threadFunction);

    MultithreadWorkerArguments* getWorkerArguments();

private:
    std::mutex argsQueueMutex;
    std::queue<MultithreadWorkerArguments*> argsQueue;

    std::mutex numWorkingThreadsMutex;
    int numWorkingThreads;
};

#endif // MULTITHREAD_WORKER_H