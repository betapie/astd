#pragma once
#ifndef ASTD_ARRAY
#define ASTD_ARRAY
#include "error.h"

namespace astd
{
    /*
    TODOs:
    Change iterator type when implemented + reverse Iterator
    Implement Swap
    implementation for _size = 0
    */

    template<typename T, size_t _size>
    struct array
    {
        using value_type = T;
        using size_type = size_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using iterator = pointer;
        using const_iterator = const_pointer;


        void fill(const T& val) // TODO can this be done at compile time?
        {
            // TODO pointer arithmetic? implement std::fill_n?
            // memset for byte-size types?
            for (size_t dataIdx = 0; dataIdx < _size; ++dataIdx)
                m_data[dataIdx] = val;
        }

        [[nodiscard]] inline T& at(size_t idx)
        {
            verify(idx < _size, "invalid array index");
            return m_data[idx];
        }

        [[nodiscard]] constexpr const T& at(size_t idx) const
        {
            verify(idx < _size, "invalid array index");
            return m_data[idx];
        }

        [[nodiscard]] inline T& operator[](size_t idx) noexcept
        {
#if _DEBUG
            verify(idx < _size, "array subscription out of range");
#endif
            return m_data[idx];
        }

        [[nodiscard]] constexpr const T& operator[](size_t idx) const noexcept
        {
#if _DEBUG
            verify(idx < _size, "array subscription out of range");
#endif
            return m_data[idx];
        }

        [[nodiscard]] inline T& front() noexcept
        {
            return m_data[0];
        }

        [[nodiscard]] constexpr const T& front() const noexcept
        {
            return m_data[0];
        }

        [[nodiscard]] inline T& back() noexcept
        {
            return m_data[_size - 1];
        }

        [[nodiscard]] constexpr const T& back() const noexcept
        {
            return m_data[_size - 1];
        }

        [[nodiscard]] inline T* data() noexcept
        {
            return m_data;
        }

        [[nodiscard]] constexpr const T* data() const noexcept
        {
            return m_data;
        }

        [[nodiscard]] inline iterator begin() noexcept
        {
            return m_data;
        }

        [[nodiscard]] inline const_iterator begin() const noexcept
        {
            return m_data;
        }

        [[nodiscard]] inline iterator end() noexcept
        {
            return m_data + _size;
        }

        [[nodiscard]] inline const_iterator end() const noexcept
        {
            return m_data + _size;
        }

        [[nodiscard]] inline const_iterator cbegin() const noexcept
        {
            return m_data;
        }

        [[nodiscard]] inline const_iterator cend() const noexcept
        {
            return m_data + _size;
        }

        [[nodiscard]] constexpr size_type size() const noexcept
        {
            return _size;
        }

        [[nodiscard]] constexpr size_type max_size() const noexcept
        {
            return _size;
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return false;
        }

        T m_data[_size];
    };

    template<size_t idx, typename T, size_t _size>
    [[nodiscard]] constexpr T& get(array<T, _size>& arr) noexcept
    {
        static_assert(idx < _size, "array index out of bounds");
        return arr.m_data[idx];
    }

    template<size_t idx, typename T, size_t _size>
    [[nodiscard]] constexpr const T& get(const array<T, _size>& arr) noexcept
    {
        static_assert(idx < _size, "array index out of bounds");
        return arr.m_data[idx];
    }

    template<size_t idx, typename T, size_t _size>
    [[nodiscard]] constexpr T&& get(array<T, _size>&& arr) noexcept
    {
        static_assert(idx < _size, "array index out of bounds");
        return move(arr.m_data[idx]);
    }

    template<size_t idx, typename T, size_t _size>
    [[nodiscard]] constexpr const T&& get(const array<T, _size>&& arr) noexcept
    {
        static_assert(idx < _size, "array index out of bounds");
        return move(arr.m_data[idx]);
    }
}
#endif //ASTD_ARRAY