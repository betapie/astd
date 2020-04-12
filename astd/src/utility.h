#pragma once
#ifndef ASTD_UTILITY
#define ASTD_UTILITY

#include "type_traits.h"

namespace astd
{
    template<typename FwdIt1, typename FwdIt2>
    constexpr void iter_swap(FwdIt1 lhs, FwdIt2 rhs)
    {
        swap(*lhs, *rhs);
    }

    template<typename T, enable_if_t<is_move_constructible_v<T> && is_move_assignable_v<T>, int> is_enabled>
    constexpr void swap(T& lhs, T& rhs) noexcept
    {
        auto temp = move(lhs);
        lhs = move(rhs);
        rhs = move(lhs);
    }

    template<typename T, size_t _size, enable_if_t<is_swappable<T>::value, int> is_enabled>
    constexpr void swap(T(&lhs)[_size], T(&rhs)[_size])
    {
        if (&lhs == &rhs)
            return;
        auto* begin = lhs;
        const auto* end = begin + _size;
        for (auto* rbegin = rhs; begin < end; ++begin, ++rbegin)
        {
            iter_swap(begin, rbegin);
        }
    }

    struct _noCopyMove_t
    {
        ~_noCopyMove_t() = delete;
        _noCopyMove_t(_noCopyMove_t const&) = delete;
        void operator=(_noCopyMove_t const&) = delete;
    };

    /* TODOs pair
        swap, get, refactor make_pair when reference_wrapper is implemented
    */


    template<typename T1, typename T2>
    struct pair
    {
        using first_type = T1;
        using second_type = T2;

        // TODO explicit overload
        template<typename U1 = T1, typename U2 = T2, enable_if_t<conjunction<is_default_constructible<U1>, is_default_constructible<U2>>::value, int> = 0>
        constexpr pair()
            : first(), second()
        {}

        // TODO explicit overload
        template<typename U1 = T2, typename U2 = T2, enable_if_t<conjunction<is_copy_constructible<U1>, is_copy_constructible<U2>>::value, int> = 0>
        pair(const T1& t1, const T2& t2)
            : first(t1), second(t2)
        {}

        // TODO explicit overload
        template<typename U1, typename U2, enable_if_t<conjunction<is_constructible<T1, U1&&>, is_constructible<T2, U2&&>>::value, int> = 0>
        pair(U1&& u1, U2&& u2)
            : first(forward<U1>(u1)), second(forward<U2>(u2))
        {}

        // TODO explicit overload
        template<typename U1, typename U2, enable_if_t<conjunction<is_constructible<T1, const U1&>, is_constructible<T1, const T2&>>::value, int> = 0>
        constexpr pair(const pair<U1, U2>& other)
            : first(other.first), second(other.second)
        {}

        // TODO explicit overload
        template<typename U1, typename U2, enable_if_t<conjunction<is_constructible<T1, U1&&>, is_constructible<T2, U2&&>>::value, int> = 0>
        constexpr pair(pair<U1, U2>&& other)
            : first(forward<U1>(other.first)), second(forward<U2>(other.second))
        {}

        // TODO piecewise construct overload

        constexpr pair(const pair& other) = default;
        constexpr pair(pair&& other) = default;

        pair& operator=(typename conditional<conjunction<is_copy_assignable<T1>, is_copy_assignable<T2>>::value,
            const pair&, _noCopyMove_t&>::type other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        template<typename U1, typename U2, enable_if_t<conjunction<is_assignable<T1&, const U1&>, is_assignable<T2&, const U2&>>::value, int> = 0>
        constexpr pair& operator=(const pair<U1, U2>& other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        // TODO noexcept with is_nothrow_move_assignable
        pair& operator=(typename conditional<conjunction<is_move_assignable<T1>, is_move_assignable<T2>>::value,
            pair&&, _noCopyMove_t&&>::type other) noexcept
        {
            first = forward<T1>(other.first);
            second = forward<T2>(other.second);
            return *this;
        }

        // TODO noexcept with is_nothrow_assignable
        template<typename U1, typename U2, enable_if_t<conjunction<is_assignable<T1&, U1&&>, is_assignable<T2&, U2&&>>::value, int> = 0>
        constexpr pair& operator=(pair<U1, U2>&& other)
        {
            first = forward<U1>(other.first);
            second = forward<U2>(other.second);
            return *this;
        }

        T1 first;
        T2 second;
    };

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second);
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return rhs < lhs;
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return !(rhs < lhs);
    }

    template<typename T1, typename T2>
    [[nodiscard]] constexpr bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return !(lhs < rhs);
    }

    // TODO, refactor when reference_wrapper is implemented
    template<typename T1, typename T2>
    pair<decay_t<T1>, decay_t<T2>> make_pair(T1&& t1, T2&& t2)
    {
        return pair<decay_t<T1>, decay_t<T2>>(forward<T1>(t1), forward<T2>(t2));
    }
}

#endif // ASTD_UTILITY