#ifndef TASKWRAPPER_H
#define TASKWRAPPER_H
#include <utility>
#include <cassert>
#include <memory>
#include "workerthreadcommands.h"

namespace TP {
    namespace Utilities {
        class TaskWrapper{
            public:
                TaskWrapper(WTCommand command) {
                    cmd = command;
                }
                virtual ~TaskWrapper() = default;
                virtual void run(){};
                WTCommand cmd;

        };

        template<typename Callable>
        class TaskWrapperImpl : public TaskWrapper{
            public:
                TaskWrapperImpl(Callable& task): TaskWrapper(WTCommand::run_task){
                    if constexpr (std::is_copy_constructible_v<Callable>) task_ = std::make_unique<Callable>(task);
                    else task_ = std::make_unique<Callable>(std::move(task));
                }
                TaskWrapperImpl(std::unique_ptr<Callable> taskptr): TaskWrapper(WTCommand::run_task), task_(std::move(taskptr)){}
                virtual ~TaskWrapperImpl() = default;
                void run() override{ if (cmd == WTCommand::run_task) (*task_)();}
            private:
                std::unique_ptr<Callable> task_;
        };
    }
}
#endif // TASKWRAPPER_H
