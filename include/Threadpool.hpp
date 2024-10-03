#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<thread>
#include<functional>
#include<mutex>
#include<condition_variable>
#include<tuple>
#include<queue>

namespace apollocpp
{
    class Threadpool{
    public:
        template<class F, class Tuple, size_t... Indices>
        void applyHelper(F&& f, Tuple&& t, std::index_sequence<Indices...>){
	        std::forward<F>(f)(std::get<Indices>(std::forward<Tuple>(t))...);
        }
    public:
        Threadpool(size_t);
        template<class F, class... Args>
        void enqueue(F&& f, Args&&... args)
        {
            {
                std::unique_lock<std::mutex> lock(mtx);
                tasks.emplace([func = std::forward<F>(f), args = std::make_tuple(std::forward<Args>(args)...), this]() {
                    this->applyHelper(std::move(func), std::move(args), std::make_index_sequence<sizeof...(Args)>{});
                    });
                cond.notify_one();
            }
        }

        void stop();
        void join();
        ~Threadpool();
    private:
        std::mutex mtx;
        std::condition_variable cond; // 条件变量
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        bool threadStop = false; // 初始化为0
    };
    
} // namespace apollocpp
