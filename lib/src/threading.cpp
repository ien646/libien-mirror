#include <ien/threading.hpp>

#include <ien/platform.hpp>

#if defined(IEN_OS_WIN)
    #include <Windows.h>
#elif defined(IEN_OS_UNIX)
    #include <pthread.h>
#else
    #error Unsupported platform for threading
#endif

namespace ien
{
    void set_current_thread_priority(thread_priority priority)
    {
        #if defined(IEN_OS_WIN)
            HANDLE hthread = GetCurrentThread();
            int priority_val = 0;
            switch(priority)
            {
                case thread_priority::VERY_LOW:
                    priority_val = THREAD_PRIORITY_LOWEST;
                    break;
                case thread_priority::LOW:
                    priority_val = THREAD_PRIORITY_BELOW_NORMAL;
                    break;
                case thread_priority::NORMAL:
                    priority_val = THREAD_PRIORITY_NORMAL;
                    break;
                case thread_priority::HIGH:
                    priority_val = THREAD_PRIORITY_ABOVE_NORMAL;
                    break;
                case thread_priority::VERY_HIGH:
                    priority_val = THREAD_PRIORITY_HIGHEST;
                    break;
            }
            SetThreadPriority(hthread, priority_val);

        #elif defined(IEN_OS_LINUX)

            pthread_t self = pthread_self();
            sched_param sparam;
            int policy;
            pthread_getschedparam(self, &policy, &sparam);
            int priority_val = 0;
            switch(priority)
            {
                case thread_priority::VERY_LOW:
                    priority_val = 19;
                    break;
                case thread_priority::LOW:
                    priority_val = 10;
                    break;
                case thread_priority::NORMAL:
                    priority_val = 0;
                    break;
                case thread_priority::HIGH:
                    priority_val = -10;
                    break;
                case thread_priority::VERY_HIGH:
                    priority_val = -20;
                    break;
            }
            sparam.sched_priority = priority_val;
            pthread_setschedparam(self, policy, &sparam);
        #endif
    }
}