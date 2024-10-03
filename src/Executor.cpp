
#include"Executor.hpp"

namespace apollocpp{
    // TODO CONFIG
    const std::shared_ptr<apollocpp::Threadpool> Executor::executor = std::make_shared<apollocpp::Threadpool>(5);
    
    void Executor::submit(std::function<void()>&& f){
        executor->enqueue(std::forward<std::function<void()>>(f));
    }

}