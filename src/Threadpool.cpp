
#include"Threadpool.hpp"

namespace apollocpp{
    
    Threadpool::Threadpool(size_t size){

        std::function<void()> func = [this](){
            while (true)
            {
                /* 线程代码 */
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cond.wait(lock, [this](){return !tasks.empty() or threadStop;});
                    if (threadStop and tasks.empty()){
                        break;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();// 执行任务
            }
        };


        // 进行创建
        for (size_t i=0;i<size;i++){
            threads.emplace_back(func); // 直接创建func为函数的thread
        }
    }

    void Threadpool::stop(){
        {
            std::unique_lock<std::mutex> lock(mtx);
            threadStop = true;
        }
        cond.notify_all();
    }

    Threadpool::~Threadpool(){
        {
            std::unique_lock<std::mutex> lock(mtx);
            threadStop = true;
        }
        // 这个代码是，将所有的线程从条件变量的等待中唤醒，
        // 然后让他们全部开始竞争锁
        cond.notify_all();
        for (size_t i=0;i<threads.size();i++){
            threads[i].join();
        }
    }

    void Threadpool::join(){
        for (auto &it:threads){
            it.join();
        }
    }
}