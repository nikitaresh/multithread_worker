
#include <thread>
#include <chrono>
#include <cstdlib>
#include <multithread_worker.h>


class TimeArgument : public MultithreadWorkerArguments
{
public:
    TimeArgument() : MultithreadWorkerArguments(), sleepForTime() {}
    TimeArgument(const TimeArgument& other) 
        : MultithreadWorkerArguments(), sleepForTime(other.sleepForTime) {}

    TimeArgument& operator=(const TimeArgument& other)
    {
        sleepForTime = other.sleepForTime;
    }

    virtual bool isWorkerExit() const
    {
        return sleepForTime == std::chrono::milliseconds(0);
    }

public:
    std::chrono::milliseconds sleepForTime;
};

void executeTask(const MultithreadWorkerArguments* ptrAbstractArguments)
{
    if (ptrAbstractArguments == nullptr || ptrAbstractArguments->isWorkerExit()) {
        return;
    }

    const TimeArgument* ptrArguments = 
        dynamic_cast<const TimeArgument*>(ptrAbstractArguments);
    if (ptrArguments == nullptr) {
        return;
    }

    std::this_thread::sleep_for(ptrArguments->sleepForTime);
}

int main()
{
    const int numTasks = 25;
    std::vector<TimeArgument> argumentsArr(numTasks);
    std::queue<MultithreadWorkerArguments*> argsQueue;
    for (size_t index = 0; index < argumentsArr.size(); ++index)
    {
        argumentsArr[index].sleepForTime = std::chrono::milliseconds(std::rand() % 1000);
        argsQueue.push(&argumentsArr[index]);
    }

    MultithreadWorker multithreadWorker;
    const int numThreads = 10;

    std::function<void(const MultithreadWorkerArguments*)> threadFunction = &executeTask;
    multithreadWorker.run(numThreads, argsQueue, threadFunction);

    std::chrono::milliseconds sleepForTime = std::chrono::milliseconds(10);
    multithreadWorker.waitUntilTheEnd(sleepForTime);

    return 0;
}
