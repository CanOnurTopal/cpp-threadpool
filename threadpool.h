#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <future>
#include "utilities/utilities.h"


namespace TP {

    template<class ExecutionPolicy = Utilities::DefaultExecutionPolicy>
    class ThreadPool{
        public:
            ThreadPool(std::size_t thread_count);
            ~ThreadPool();
            ThreadPool(const ThreadPool&) = delete;
            ThreadPool& operator=(ThreadPool const&) = delete;
            ThreadPool(ThreadPool<ExecutionPolicy>&&) noexcept;
            ThreadPool<ExecutionPolicy>& operator=(ThreadPool<ExecutionPolicy>&&) noexcept;


            bool set_thread_count(std::size_t count);
            size_t get_thread_count();

            template<typename Callable, typename... Args>
            std::future<typename Utilities::p_invoke_result_t<Callable, Args...>> submit(Callable, Args...);

            template<typename Callable, typename... Args>
            void submit_nf(Callable, Args...);

        private:
            inline void submit_task(std::shared_ptr<Utilities::TaskWrapper>);
            inline void submit_task(Utilities::TaskWrapper*);
            inline void submit_worker_deleter(std::size_t count = 1);

            inline void clear_empty_threads();
            inline void join_all_threads();

            std::shared_ptr<ExecutionPolicy> ExecutionPolicy_;
            std::vector<std::shared_ptr<std::thread>> threads_;
    };
}

#include "threadpool.cpp"

#endif // THREADPOOL_H
