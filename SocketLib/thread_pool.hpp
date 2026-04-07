#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>

class ThreadPool {
public:
    explicit ThreadPool(int num_threads) {
        for (int i = 0; i < num_threads; i++) {
            _workers.emplace_back([this] { worker_loop(); });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _stop = true;
        }
        _cv.notify_all();

        for (auto& t : _workers)
            t.join();
    }

    void submit(std::function<void()> job) {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _queue.push(std::move(job));
        }
        _cv.notify_one();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void worker_loop() {
        while (true) {
            std::function<void()> job;

            {
                std::unique_lock<std::mutex> lock(_mutex);

                _cv.wait(lock, [this] {
                    return !_queue.empty() || _stop;
                });

                if (_stop && _queue.empty())
                    return;

                job = std::move(_queue.front());
                _queue.pop();
            }

            job(); 
        }
    }

    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _queue;
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _stop = false;
};
