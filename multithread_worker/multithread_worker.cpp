
#include <multithread_worker.h>
#include <thread>

MultithreadWorkerArguments::~MultithreadWorkerArguments()
{
}


MultithreadWorker::MultithreadWorker()
    : argsQueueMutex(), argsQueue(), numWorkingThreadsMutex(), numWorkingThreads(0)
{
}

MultithreadWorker::~MultithreadWorker()
{
    if (numWorkingThreads != 0)
    {
        argsQueueMutex.lock();
        argsQueue = std::queue<MultithreadWorkerArguments*>();
        argsQueueMutex.unlock();
    }

    waitUntilTheEnd(std::chrono::milliseconds(1));
}

bool MultithreadWorker::run(int numThreads,
                            const std::queue<MultithreadWorkerArguments*>& argumentQueue,
                            std::function<void(const MultithreadWorkerArguments*)>& threadFunction)
{
    if (numWorkingThreads != 0) {
        return false;
    }

    argsQueue = argumentQueue;

    numWorkingThreads = numThreads;
    for (int index = 0; index < numWorkingThreads; ++index)
    {
        std::thread worker(&MultithreadWorker::threadWorker, this, threadFunction);
        worker.detach();
    }

    return true;
}

void MultithreadWorker::waitUntilTheEnd(const std::chrono::milliseconds& sleepForTime) const
{
    while (numWorkingThreads != 0)
    {
        std::this_thread::sleep_for(sleepForTime);
    }
}

void MultithreadWorker::threadWorker(std::function<void(const MultithreadWorkerArguments*)>& threadFunction)
{
    while (true)
    {
        MultithreadWorkerArguments* arguments = getWorkerArguments();
        if (arguments == nullptr || arguments->isWorkerExit()) {
            break;
        }

        threadFunction(arguments);
    }

    numWorkingThreadsMutex.lock();
    --numWorkingThreads;
    numWorkingThreadsMutex.unlock();
}

MultithreadWorkerArguments* MultithreadWorker::getWorkerArguments()
{
    MultithreadWorkerArguments* arguments = nullptr;

    argsQueueMutex.lock();
    if (!argsQueue.empty())
    {
        arguments = argsQueue.back();
        argsQueue.pop();
    }
    argsQueueMutex.unlock();

    return arguments;
}
