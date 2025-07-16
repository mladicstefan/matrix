//
// shh - C++ WebServer
// author: mladicstefan
// repo: https://github.com/mladicstefan/shhh
// LISCENSE: MIT
//

#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <iostream>

namespace shh{

class ThreadPool{
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop{false};

public:
    explicit ThreadPool(size_t n = std::thread::hardware_concurrency()){
        for (size_t i = 0; i < n; i++){
            workers_.emplace_back([this] {
                while (true){
                    std::function<void()> task;
                    {
                        std::unique_lock lk(queue_mutex_);
                        cv_.wait(lk, [&] { return stop || !tasks_.empty();});
                        if (stop && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    try {
                        task();
                    }
                    catch (std::exception& e){
                        std::cerr << "Threadpool constructor error: " << e.what() << std::endl;
                    }
                }
            });
        }
    }

    ~ThreadPool(){
        stop = true;
        cv_.notify_all();
        for (auto& t : workers_){
            if (t.joinable()) t.join();
        }
    }

    template<class F>
    //foward f for lvalue and rvalue
    void enqueue(F&& f) {
        {
        std::lock_guard lk(queue_mutex_);
        tasks_.emplace(std::forward<F>(f));
        }
        cv_.notify_one();
    }
};
}
