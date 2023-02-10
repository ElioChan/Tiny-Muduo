#include "ThreadPoll.h"

ThreadPoll::ThreadPoll(int size) : stop(false){
    for(int i = 0; i < size; ++i) {
        threads.emplace_back(std::thread([this](){
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    cv.wait(lock, [this](){
                        return stop || !tasks.empty();
                    });
                    if(stop && tasks.empty()) return;
                    task = tasks.front();
                    tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPoll::~ThreadPoll(){
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();
    for(std::thread &th : threads) {
        if(th.joinable()) th.join();  // 回收资源
    }
}

void ThreadPoll::add(std::function<void()> func) {
    {
        // 增加任务队列 需要加锁
        std::unique_lock<std::mutex> lock(tasks_mtx);
        if(stop) throw std::runtime_error("ThreadPoll already sop, can't add task any more!");
        tasks.emplace(func);
    }
    cv.notify_one();
}