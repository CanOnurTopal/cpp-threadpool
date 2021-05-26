#ifndef UTILITIES_H
#define UTILITIES_H

#include "portable_invoke_result.h"
#include "fifopolicy.h"
#include "taskwrapper.h"
#include "workerthread.h"
namespace TP {
    namespace Utilities {
        using DefaultExecutionPolicy = FIFOPolicy;

        template<typename ExecutionPolicy = DefaultExecutionPolicy>
        void worker_thread(std::shared_ptr<ExecutionPolicy> policy);
    }

}


#endif // UTILITIES_H
