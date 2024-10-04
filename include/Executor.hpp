#pragma once
#include"Threadpool.hpp"
#include<memory>

namespace apollocpp{
    class Executor{
    public:
        static const std::shared_ptr<apollocpp::Threadpool> executor;
        template<class F, class... Args>
        static void submit(F&& f, Args&&... args){
            executor->enqueue(std::forward<F>(f), std::forward<Args>(args)...);
        }
        static void submit(std::function<void()>&&);
    };
}