#include <ien/activity.hpp>

#include <ien/platform.hpp>

#ifdef IEN_OS_WIN
    #include <ien/win32/windows.h>
#endif

#include <cstdlib>

namespace ien
{
    void simulate_user_activity()
    {
    #ifdef IEN_OS_LINUX
        system("qdbus org.freedesktop.ScreenSaver /ScreenSaver SimulateUserActivity > /dev/null");
    #elif defined(IEN_OS_WIN)
        SetThreadExecutionState(ES_DISPLAY_REQUIRED);
    #endif
    }
}