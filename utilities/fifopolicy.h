#ifndef FIFOPOLICY_H
#define FIFOPOLICY_H
#include <memory>
#include <queue>
#include <mutex>
#include "taskwrapper.h"

namespace TP{
    namespace Utilities {
        class FIFOPolicy{
            public:
                //FIFOPolicy();
                void add_task(std::shared_ptr<TaskWrapper>);
                std::shared_ptr<TaskWrapper> get_task();
                std::size_t task_count();

            private:
                std::mutex qm_;
                std::queue<std::shared_ptr<TaskWrapper>> queue_;
        };
    }
}


#endif // FIFOPOLICY_H
