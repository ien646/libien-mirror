#pragma once

#include <thread>

namespace ien
{
    enum class thread_priority
    {
        VERY_LOW,
        LOW,
        NORMAL,
        HIGH,
        VERY_HIGH
    };

    extern void set_current_thread_priority(thread_priority priority);
}