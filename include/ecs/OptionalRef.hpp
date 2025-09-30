#pragma once

#include <optional>
#include <utility>

template <typename T> class OptionalRef
{
  public:
    OptionalRef() noexcept : m_Ptr(nullptr) {}
    OptionalRef(std::nullopt_t) noexcept : m_Ptr(nullptr) {}
    OptionalRef(T& ref) noexcept : m_Ptr(std::addressof(ref)) {}
    OptionalRef(T* ptr) noexcept : m_Ptr(ptr) {}

    inline constexpr bool has_value() const noexcept { return m_Ptr != nullptr; }
    inline constexpr explicit operator bool() const noexcept { return has_value(); }

    inline constexpr T& value()
    {
        if (!m_Ptr)
            throw std::bad_optional_access{};
        return *m_Ptr;
    }
    inline constexpr const T& value() const
    {
        if (!m_Ptr)
            throw std::bad_optional_access{};
        return *m_Ptr;
    }

    inline constexpr T* operator->() { return m_Ptr; }
    inline constexpr const T* operator->() const { return m_Ptr; }

    inline constexpr T& operator*() { return *m_Ptr; }
    inline constexpr const T& operator*() const { return *m_Ptr; }

  private:
    T* m_Ptr;
};
