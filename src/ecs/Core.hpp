#pragma once

#include "PlatformDetection.hpp"

#ifdef ECS_DEBUG
#ifdef ECS_PLATFORM_WINDOWS
#define ECS_DEBUG_BREAK() __debugbreak()
#elif defined(ECS_PLATFORM_LINUX) || defined(ECS_PLATFORM_ANDROID)
#include <signal.h>
#define ECS_DEBUG_BREAK() raise(SIGTRAP)
#endif // ECS_PLATFORM_WINDOWS

#include <iostream>
#define ECS_ASSERT(x)                                                                                                                      \
    {                                                                                                                                      \
        if (!(x))                                                                                                                          \
        {                                                                                                                                  \
            std::cout << "Assertion Failed" << std::endl;                                                                                  \
            ECS_DEBUG_BREAK();                                                                                                             \
        }                                                                                                                                  \
    }
#else
#define ECS_ASSERT(x)
#endif // ECS_DEBUG
