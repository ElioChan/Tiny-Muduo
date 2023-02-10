#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPoll
{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;  // 为什么task类型中的function是void?
    std::mutex tasks_mtx;
    std::condition_variable cv;
    bool stop;
public:
    ThreadPoll(int size = std::thread::hardware_concurrency());
    ~ThreadPoll();

    void add(std::function<void()>);
};