#pragma once

#include "PlatformDetection.hpp"
#include <cstddef>
#include <cstring>
#include <functional>
#include <vector>
#include <array>
#include <algorithm>

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
    constexpr size_t DefaultChunkSize        = 16384;
    constexpr size_t DefaultCommandQueueSize = 4096;

    template <typename T> inline Byte* ToBytes(T& value) { return reinterpret_cast<Byte*>(&value); }
    template <typename T> inline Byte* ToBytes(T* value) { return reinterpret_cast<Byte*>(value); }

    template <typename T> inline const Byte* ToBytes(const T& value) { return reinterpret_cast<const Byte*>(&value); }
    template <typename T> inline const Byte* ToBytes(const T* value) { return reinterpret_cast<const Byte*>(value); }

    template <typename T> inline T* FromBytes(Byte* bytes) { return reinterpret_cast<T*>(bytes); }
    template <typename T> inline const T* FromBytes(const Byte* bytes) { return reinterpret_cast<const T*>(bytes); }

    template <typename... T> inline constexpr size_t SizeOf = (sizeof(T) + ...);

    template <typename T> T inline PostAdd(T& value, T diff)
    {
        T temp = value;
        value += diff;
        return temp;
    }

    template <typename... T> inline void CopyInto(std::vector<Byte>& buffer, Index& index, const T&... items)
    {
        (std::memmove(&buffer[PostAdd(index, sizeof(T))], &items, sizeof(T)), ...);
    }

    template <typename... T> inline std::vector<Byte> CombineBytes(const T&... items)
    {
        std::vector<Byte> data(SizeOf<T...>);
        Index index = 0;
        CopyInto(data, index, items...);
        return data;
    }

    template <typename... T> std::vector<Byte> CombineBytesById(const T&... items)
    {
        struct ItemEntry
        {
            const void* ptr;
            size_t size;
            size_t type_id;
        };

        auto entries = std::array<ItemEntry, sizeof...(T)>{ ItemEntry{ &items, sizeof(T), T::GetType().Get() }... };
        std::sort(entries.begin(), entries.end(), [](auto const& a, auto const& b) { return a.type_id < b.type_id; });

        std::vector<Byte> data(SizeOf<T...>);
        size_t offset = 0;
        for (auto const& e : entries)
        {
            std::memcpy(data.data() + offset, e.ptr, e.size);
            offset += e.size;
        }

        return data;
    }
} // namespace EVA::ECS
