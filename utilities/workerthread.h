#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <memory>
#include <iostream>
#include <thread>

#include "workerthreadcommands.h"
#include "taskwrapper.h"

namespace TP {
    namespace Utilities {
        template<typename ExecutionPolicy>
        void worker_thread(std::shared_ptr<ExecutionPolicy> policy){
            try
            {
                while(true){
                    std::shared_ptr<TaskWrapper> task = policy->get_task();

                    while (!task){ //Wait for available task
                        std::this_thread::yield();
                        task = policy->get_task();
                    }

                    switch (task->cmd) {
                        case WTCommand::terminate_thread:
                            return;
                        case WTCommand::run_task:
                            task->run();
                            break;
                    }
                }
            }
            catch(...) // TODO Implement a more granular exception handler
            {
                auto expPtr = std::current_exception();
                std::cout << "exception" << std::endl;;
                try
                {
                    if(expPtr) std::rethrow_exception(expPtr);
                }
                catch(const std::exception& e)
                {
                    std::cout << e.what();
                }
            }
        }
    }
}



#endif // WORKERTHREAD_H
