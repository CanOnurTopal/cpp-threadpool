#ifndef PORTABLE_INVOKE_RESULT_H
#define PORTABLE_INVOKE_RESULT_H
#include <type_traits>

namespace TP {
    namespace Utilities {
        template<typename Callable, typename... Args>
        struct p_invoke_result{
            #if __cplusplus >= 201700
                using type = typename std::invoke_result<Callable, Args...>::type;
            #else
                using type = typename std::result_of<Callable(Args...)>::type;
            #endif
        };

        template<typename Callable, typename... Args>
        using p_invoke_result_t = typename p_invoke_result<Callable, Args...>::type;
    }

}

#endif // PORTABLE_INVOKE_RESULT_H
