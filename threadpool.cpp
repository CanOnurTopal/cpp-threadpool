#ifndef THREADPOOL_CPP
#define THREADPOOL_CPP

#include "threadpool.h"

#include <functional>
#include <future>
#include <memory>
namespace TP {

    template<class ExecutionPolicy>
    ThreadPool<ExecutionPolicy>::ThreadPool(std::size_t thread_count){
        ExecutionPolicy_ = std::make_shared<ExecutionPolicy>();
        set_thread_count(thread_count);
    }

    template<class ExecutionPolicy>
    ThreadPool<ExecutionPolicy>::~ThreadPool(){
        set_thread_count(0);
        join_all_threads();
        return;
    }

    template<class ExecutionPolicy>
    template<typename Callable, typename... Args>
    std::future<typename Utilities::p_invoke_result_t<Callable, Args...>> ThreadPool<ExecutionPolicy>::submit(Callable func, Args... args){
        using ReturnType = typename Utilities::p_invoke_result_t<Callable, Args...>;
        std::unique_ptr<std::packaged_task<ReturnType()>> task_package;
        if constexpr(sizeof... (args) == 0) task_package = std::make_unique<std::packaged_task<ReturnType()>>(func);
        else task_package = std::make_unique<std::packaged_task<ReturnType()>>(std::bind(func, args...));
        std::future<ReturnType> task_future = task_package->get_future();
        std::shared_ptr<Utilities::TaskWrapper> wrapped_task = std::make_shared<Utilities::TaskWrapperImpl<std::packaged_task<ReturnType()>>>(std::move(task_package));
        ThreadPool<ExecutionPolicy>::submit_task(std::move(wrapped_task));
        return task_future;
    }

    template<class ExecutionPolicy>
    template<typename Callable, typename... Args>
    void ThreadPool<ExecutionPolicy>::submit_nf(Callable func, Args... args){
        std::shared_ptr<Utilities::TaskWrapper> wrapped_task;
        if constexpr(sizeof... (args) == 0) wrapped_task = std::make_shared<Utilities::TaskWrapperImpl<Callable>>(func);
        else {
            using boundtype = decltype (std::bind(func, args...));
            auto boundfunc = std::make_unique<boundtype>(std::bind(func, args...));
            wrapped_task = std::make_shared<Utilities::TaskWrapperImpl<boundtype>>(std::move(boundfunc));
        }
        ThreadPool<ExecutionPolicy>::submit_task(std::move(wrapped_task));
        return;
    }

    template<class ExecutionPolicy>
    void ThreadPool<ExecutionPolicy>::submit_task(std::shared_ptr<Utilities::TaskWrapper> task_ptr){
        ThreadPool<ExecutionPolicy>::ExecutionPolicy_->add_task(std::move(task_ptr));
    }

    template<class ExecutionPolicy>
    void ThreadPool<ExecutionPolicy>::submit_worker_deleter(std::size_t count){
        auto deleter_task = std::make_shared<Utilities::TaskWrapper>(Utilities::WTCommand::terminate_thread);
        for (std::size_t x = 0; x < count; ++x){
            submit_task(deleter_task);
        }
    }

    template<class ExecutionPolicy>
    void ThreadPool<ExecutionPolicy>::clear_empty_threads(){
        for (std::size_t x = 0; x < threads_.size(); ++x){
            if (!threads_[x]->joinable()) {
                threads_.erase(threads_.begin() + x);
            }
        }
    }

    template<class ExecutionPolicy>
    bool ThreadPool<ExecutionPolicy>::set_thread_count(std::size_t new_count){
        clear_empty_threads();
        auto thread_count = threads_.size();
        if (new_count == thread_count) return true;
        else if (new_count > thread_count){
            for (std::size_t x = 0; x < (new_count - thread_count); ++x){
                threads_.push_back(std::make_shared<std::thread>(Utilities::worker_thread<ExecutionPolicy>, ExecutionPolicy_));
            }
            return true;
        }
        else {
            //new_count is less than existing thread count
            submit_worker_deleter(thread_count - new_count);
            return false;
        }
    }

    template<class ExecutionPolicy>
    void ThreadPool<ExecutionPolicy>::join_all_threads(){ //TODO Remove busy wait
        for (std::shared_ptr<std::thread> thread_ptr : threads_){
            if(thread_ptr->joinable()) thread_ptr->join();
        }
        return;
    }

    template<class ExecutionPolicy>
    ThreadPool<ExecutionPolicy>::ThreadPool(ThreadPool<ExecutionPolicy>&& other) noexcept{
        ExecutionPolicy_.swap(other.ExecutionPolicy_);
        other.ExecutionPolicy_ = std::make_shared<ExecutionPolicy>();
        threads_.swap(other.threads_);
    }

    template<class ExecutionPolicy>
    ThreadPool<ExecutionPolicy>& ThreadPool<ExecutionPolicy>::operator=(ThreadPool<ExecutionPolicy>&& other) noexcept{
        if (this!= &other){
            ExecutionPolicy_.swap(other.ExecutionPolicy_);
            other.ExecutionPolicy_ = std::make_shared<ExecutionPolicy>();
            threads_.swap(other.threads_);
        }
        return *this;
    }

    template<class ExecutionPolicy>
    size_t ThreadPool<ExecutionPolicy>::get_thread_count(){
        return threads_.size();
    }
}

#endif //THREADPOOL_CPP
