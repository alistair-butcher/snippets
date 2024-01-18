#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>

class JobQueue
{
	using fp = std::function<void(void)>;
public:
	JobQueue(int nThreads);
	JobQueue(const JobQueue&) = delete;
	JobQueue(JobQueue&&) = delete;
	JobQueue& operator=(const JobQueue&) = delete;
	JobQueue& operator=(JobQueue&&) = delete;
	void Add(fp&& fun);
private:
	std::vector<std::jthread> mThreads;
	std::mutex mQueueMutex;
	std::queue<fp> mQueue;
	std::condition_variable_any mQueueCondition;
	void ThreadHandler(std::stop_token stopToken);
};

