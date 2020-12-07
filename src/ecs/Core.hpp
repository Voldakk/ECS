#pragma once

#include "PlatformDetection.hpp"
#include <cstddef>
#include <functional>

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

namespace EVA::ECS
{
    using Index    = size_t;
    using EntityId = size_t;

    using Byte = unsigned char;
    static_assert(sizeof(Byte) == 1);
    constexpr size_t DefaultChunkSize = 16384;

    template <typename T> Byte* ToBytes(T& value) { return reinterpret_cast<Byte*>(&value); }
    template <typename T> Byte* ToBytes(T* value) { return reinterpret_cast<Byte*>(value); }

    template <typename T> const Byte* ToBytes(const T& value) { return reinterpret_cast<const Byte*>(&value); }
    template <typename T> const Byte* ToBytes(const T* value) { return reinterpret_cast<const Byte*>(value); }

    template <typename T> T PostAdd(T& value, T diff)
    {
        T temp = value;
        value += diff;
        return temp;
    }

    template <typename... T> std::vector<Byte> CombineBytes(const T&... items)
    {
        constexpr size_t size = (sizeof(T) + ...);
        std::vector<Byte> data(size);
        size_t index = 0;
        (std::memmove(&data[PostAdd(index, sizeof(T))], &items, sizeof(T)), ...);
        return data;
    }

    template <typename T> T* FromBytes(Byte* bytes) { return reinterpret_cast<T*>(bytes); }
    template <typename T> const T* FromBytes(const Byte* bytes) { return reinterpret_cast<const T*>(bytes); }
} // namespace EVA::ECS
