#include "JobQueue.h"

JobQueue::JobQueue(int nThreads)
{
	mThreads.reserve(nThreads);
	for (auto i = 0; i < nThreads; i++) {
		mThreads.emplace_back([this](std::stop_token stopToken) { ThreadHandler(stopToken); });
	}
}

void JobQueue::Add(fp&& fun)
{
	std::scoped_lock<std::mutex> lock(mQueueMutex);
	mQueue.push(std::move(fun));
	mQueueCondition.notify_one();
}

void JobQueue::ThreadHandler(std::stop_token stopToken)
{
	std::unique_lock<std::mutex> lock(mQueueMutex);
	do {
		if (!mQueueCondition.wait(lock, stopToken, 
			[this] { return !mQueue.empty(); })) {
			break;
		}
		if (!mQueue.empty()) {
			auto fun = std::move(mQueue.front());
			mQueue.pop();
			lock.unlock();

			fun();

			lock.lock();
		}
	} while (!stopToken.stop_requested());
}
