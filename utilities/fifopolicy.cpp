#include "fifopolicy.h"

namespace TP {
    namespace Utilities {
        void FIFOPolicy::add_task(std::shared_ptr<TaskWrapper> task){
            std::lock_guard<std::mutex> lock (qm_);
            queue_.push(std::move(task));
            return;
        }

        std::shared_ptr<TaskWrapper> FIFOPolicy::get_task(){
            std::lock_guard<std::mutex> lock (qm_);
            if(queue_.size() == 0) return nullptr;
            std::shared_ptr<TaskWrapper>task(queue_.front());
            queue_.pop();
            return task;
        }
    }
}
