#pragma once
#include"Threadpool.hpp"
#include<memory>

namespace apollocpp{
    class Executor{
    public:
        // 内置线程池，直接执行器，全局执行器，均可拿到此执行器使用
        static const std::shared_ptr<apollocpp::Threadpool> executor;
        template<class F, class... Args>
        static void submit(F&& f, Args&&... args){
            // 小技巧
            executor->enqueue(std::forward<F>(f), std::forward<Args>(args)...);
        }
        static void submit(std::function<void()>&&);
    };
}