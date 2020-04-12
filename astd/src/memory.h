#pragma once
#ifndef ASTD_MEMORY
#define ASTD_MEMORY

#include "cstddef.h"
#include "type_traits.h"

namespace astd
{
    template<typename T>
    struct default_delete
    {
        constexpr default_delete() noexcept = default;

        template<typename U, enable_if_t<is_convertible_v<T*, U*>, int> = 0>
        default_delete(const default_delete<U>& other) noexcept
        {}

        void operator()(T* ptr) const
        {
            static_assert(sizeof(T) > 0, "cannot delete incomplete type");
            delete ptr;
        }
    };

    template<typename T>
    struct default_delete<T[]>
    {
        constexpr default_delete() noexcept = default;

        template<typename U, enable_if_t<is_convertible_v<T(*)[], U(*)[]>, int> = 0>
        default_delete(const default_delete<U>& other) noexcept
        {}

        void operator()(T* ptr) const
        {
            static_assert(sizeof(T) > 0, "cannot delete incomplete type");
            delete[] ptr;
        }
    };

    template<typename T, typename Deleter = default_delete<T>>
    class unique_ptr
    {
    private:
        template<typename U, typename V, typename = void>
        struct _ptr_type_impl
        {
            using type = U*;
        };

        template<typename U, typename V>
        struct _ptr_type_impl<U, V, void_t<typename remove_reference_t<V>::pointer>>
        {
            using type = typename remove_reference_t<V>::pointer;
        };

    public:
        using pointer = typename _ptr_type_impl<T, Deleter>::type;
        using element_type = T;
        using deleter_type = Deleter;

        template<typename Del = Deleter, enable_if_t<conjunction_v<is_default_constructible<Del>, negation<is_pointer<Del>>>, int> = 0>
        constexpr unique_ptr() noexcept
            :m_ptr{}, m_del{}
        {}

        template<typename Del = Deleter, enable_if_t<conjunction_v<is_default_constructible<Del>, negation<is_pointer<Del>>>, int> = 0>
        constexpr unique_ptr(nullptr_t) noexcept
            :m_ptr{}, m_del{}
        {}

        template<typename Del = Deleter, enable_if_t<conjunction_v<is_default_constructible<Del>, negation<is_pointer<Del>>>, int> = 0>
        explicit unique_ptr(pointer ptr) noexcept
            :m_ptr(ptr), m_del{}
        {}

        template<typename Del = deleter_type, enable_if_t<is_copy_constructible_v<Del>, int> = 0>
        unique_ptr(pointer ptr, const Del& del) noexcept
            :m_ptr(ptr), m_del(del)
        {}

        template<typename Del = deleter_type, enable_if_t<conjunction_v<negation<is_reference<Del>>, is_move_constructible<Del>>, int> = 0>
        unique_ptr(pointer ptr, Del&& del)
            : m_ptr(ptr), m_del(forward<Del>(del))
        {}

        template<typename Del = deleter_type, enable_if_t<conjunction_v<is_reference<Del>, is_constructible<Del, remove_reference_t<Del>>>, int> = 0>
        unique_ptr(pointer ptr, Del&& del) = delete;

        unique_ptr(const unique_ptr& other) = delete;
        unique_ptr& operator=(const unique_ptr& other) = delete;

        template<typename Del = deleter_type, enable_if_t<is_move_constructible_v<Del>, int> = 0>
        unique_ptr(unique_ptr&& other) noexcept
            : m_ptr(other.release()), m_del(forward<Del>(other.m_del))
        {}

        template<typename T2, typename Deleter2,
            enable_if_t<conjunction_v<
            conditional_t<is_reference_v<Deleter2>, is_same<Deleter, Deleter2>, is_convertible<Deleter2, Deleter>>,
            is_convertible<typename unique_ptr<T2, Deleter2>::pointer, pointer>,
            negation<is_array<T2>>
            >,
            int> = 0>
        unique_ptr(unique_ptr<T2, Deleter2>&& other) noexcept
            :m_ptr(other.release()), m_del(forward<Deleter2>(other.m_del))
        {}

        unique_ptr& operator=(nullptr_t)
        {
            reset();
            return *this;
        }

        template<typename Del = deleter_type, enable_if_t<is_move_assignable_v<Del>, int> = 0>
        unique_ptr& operator=(unique_ptr&& other) noexcept
        {
            if (this == &other)
                return *this;

            reset(other.release());
            m_del = forward<Del>(other.m_del);
            return *this;
        }

        template<typename T2, typename Deleter2,
            enable_if_t<conjunction_v<negation<is_array<T2>>,
            is_convertible<typename unique_ptr<T2, Deleter2>::pointer, pointer>,
            is_assignable<Deleter&, Deleter2&&>>, int> = 0>
        unique_ptr& operator=(unique_ptr<T2, Deleter2>&& other)
        {
            if (this == &other)
                return *this;

            reset(other.release());
            m_del = forward<Deleter2>(other.m_del);
            return *this;
        }

        ~unique_ptr()
        {
            if (m_ptr)
            {
                m_del(m_ptr);
            }
        }

        [[nodiscard]] deleter_type& get_deleter() noexcept
        {
            return m_del;
        }

        [[nodiscard]] const deleter_type& get_deleter() const noexcept
        {
            return m_del;
        }

        [[nodiscard]] pointer operator->() const noexcept
        {
            return m_ptr;
        }

        [[nodiscard]] pointer get() const noexcept
        {
            return m_ptr;
        }

        [[nodiscard]] add_lvalue_reference_t<T> operator*() const noexcept
        {
            return *m_ptr;
        }

        explicit operator bool() const noexcept
        {
            return m_ptr != nullptr;
        }

        pointer release() noexcept // TODO replace by std::exchange
        {
            const auto retval = m_ptr;
            m_ptr = nullptr;
            return retval;
        }

        void reset(pointer ptr = pointer())
        {
            const auto cur = m_ptr;
            m_ptr = ptr;
            if (cur)
            {
                m_del(cur);
            }
        }

    private:
        template<typename, typename>
        friend class unique_ptr;

        T* m_ptr = nullptr;
        Deleter m_del;
    };

    template<typename T, typename Deleter>
    class unique_ptr<T[], Deleter>
    {
    private:
        template<typename U, typename V, typename = void>
        struct _ptr_type_impl
        {
            using type = U*;
        };

        template<typename U, typename V>
        struct _ptr_type_impl<U, V, void_t<typename remove_reference_t<V>::pointer>>
        {
            using type = typename remove_reference_t<V>::pointer;
        };

    public:
        using pointer = typename _ptr_type_impl<T, Deleter>::type;
        using element_type = T;
        using deleter_type = Deleter;

        template<typename Del = Deleter, enable_if_t<conjunction_v<is_default_constructible<Del>, negation<is_pointer<Del>>>, int> = 0>
        constexpr unique_ptr() noexcept
            :m_ptr{}, m_del{}
        {}

        template<typename Del = Deleter, enable_if_t<conjunction_v<is_default_constructible<Del>, negation<is_pointer<Del>>>, int> = 0>
        constexpr unique_ptr(nullptr_t) noexcept
            :m_ptr{}, m_del{}
        {}

        template<typename U>
        struct _array_constructible_impl : bool_constant<disjunction_v<is_same<U, pointer>, is_same<U, nullptr_t>,
            conjunction<is_same<U, element_type*>, is_pointer<U>, is_convertible<remove_pointer_t<U>(*)[], element_type(*)[]>>>>
        {};

        template<typename U, typename Del = Deleter, enable_if_t<conjunction_v<is_default_constructible<Del>, negation<is_pointer<Del>>,
            _array_constructible_impl<U>>, int> = 0>
        explicit unique_ptr(U ptr) noexcept
            :m_ptr(ptr), m_del{}
        {}

        template<typename U, typename Del = deleter_type, enable_if_t<conjunction_v<is_copy_constructible<Del>, _array_constructible_impl<U>>, int> = 0>
        unique_ptr(U ptr, const Del& del) noexcept
            :m_ptr(ptr), m_del(del)
        {}

        template<typename U, typename Del = deleter_type, enable_if_t<conjunction_v<
            negation<is_reference<Del>>, is_move_constructible<Del>, _array_constructible_impl<U>>, int> = 0>
        unique_ptr(U ptr, Del&& del)
            : m_ptr(ptr), m_del(forward<Del>(del))
        {}

        template<typename Del = deleter_type, enable_if_t<conjunction_v<is_reference<Del>, is_constructible<Del, remove_reference_t<Del>>>, int> = 0>
        unique_ptr(pointer ptr, Del&& del) = delete;

        unique_ptr(const unique_ptr& other) = delete;
        unique_ptr& operator=(const unique_ptr& other) = delete;

        template<typename Del = deleter_type, enable_if_t<is_move_constructible_v<Del>, int> = 0>
        unique_ptr(unique_ptr&& other) noexcept
            : m_ptr(other.release()), m_del(forward<Del>(other.m_del))
        {}

        template<typename T2, typename Deleter2,
            enable_if_t<conjunction_v<
            conditional_t<is_reference_v<Deleter2>, is_same<Deleter, Deleter2>, is_convertible<Deleter2, Deleter>>,
            is_same<pointer, element_type*>,
            is_same<typename unique_ptr<T2, Deleter2>::pointer, typename unique_ptr<T2, Deleter2>::element_type*>,
            is_convertible<typename unique_ptr<T2, Deleter2>::element_type(*)[], element_type(*)[]>,
            is_array<T2>>,
            int> = 0>
        unique_ptr(unique_ptr<T2, Deleter2>&& other) noexcept
            :m_ptr(other.release()), m_del(forward<Deleter2>(other.m_del))
        {}

        unique_ptr& operator=(nullptr_t)
        {
            reset();
            return *this;
        }

        template<typename Del = deleter_type, enable_if_t<is_move_assignable_v<Del>, int> = 0>
        unique_ptr& operator=(unique_ptr&& other) noexcept
        {
            if (this == &other)
                return *this;

            reset(other.release());
            m_del = forward<Del>(other.m_del);
            return *this;
        }

        template<typename T2, typename Deleter2,
            enable_if_t<conjunction_v<is_array<T2>,
            is_same<pointer, element_type*>,
            is_same<typename unique_ptr<T2, Deleter2>::pointer, typename unique_ptr<T2, Deleter2>::element_type*>,
            is_convertible<typename unique_ptr<T2, Deleter2>::element_type(*)[], element_type(*)[]>,
            is_assignable<Deleter&, Deleter2&&>>, int> = 0>
        unique_ptr& operator=(unique_ptr<T2, Deleter2>&& other)
        {
            if (this == &other)
                return *this;

            reset(other.release());
            m_del = forward<Deleter2>(other.m_del);
            return *this;
        }

        ~unique_ptr()
        {
            if (m_ptr)
            {
                m_del(m_ptr);
            }
        }

        [[nodiscard]] deleter_type& get_deleter() noexcept
        {
            return m_del;
        }

        [[nodiscard]] const deleter_type& get_deleter() const noexcept
        {
            return m_del;
        }

        [[nodiscard]] pointer get() const noexcept
        {
            return m_ptr;
        }

        [[nodiscard]] add_lvalue_reference_t<T> operator[](size_t idx) const noexcept
        {
            return m_ptr[idx];
        }

        explicit operator bool() const noexcept
        {
            return m_ptr != nullptr;
        }

        pointer release() noexcept // TODO replace by std::exchange
        {
            const auto retval = m_ptr;
            m_ptr = nullptr;
            return retval;
        }

        void reset(pointer ptr = pointer())
        {
            const auto cur = m_ptr;
            m_ptr = ptr;
            if (cur)
            {
                m_del(cur);
            }
        }

        template<typename U, enable_if_t<_array_constructible_impl<U>::value, int> = 0>
        void reset(U ptr)
        {
            const auto cur = m_ptr;
            m_ptr = ptr;
            if (cur)
            {
                m_del(cur);
            }
        }

        void reset(nullptr_t)
        {
            reset(pointer());
        }

    private:
        template<typename, typename>
        friend class unique_ptr;

        T* m_ptr = nullptr;
        Deleter m_del;
    };

    template<typename T, typename... Args, enable_if_t<!is_array_v<T>, int> = 0>
    [[nodiscard]] unique_ptr<T> make_unique(Args... args)
    {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }

    template<typename T, enable_if_t<is_array_v<T> && extent_v<T> == 0, int> = 0>
    [[nodiscard]] unique_ptr<T> make_unique(size_t size)
    {
        return unique_ptr<T>(new remove_extent_t<T>[size]);
    }

    template<typename T, typename... Args, enable_if_t<extent_v<T> != 0, int> = 0>
    unique_ptr<T> make_unique(Args...) = delete;
}

#endif // ASTD_MEMORY