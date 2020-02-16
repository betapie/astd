#pragma once
#ifndef ASTD_MEMORY
#define ASTD_MEMORY

#include "cstddef.h"
#include "type_traits.h"

namespace astd
{
    template <typename T, enable_if_t<!is_array<T>::value, int> = 0>
    class unique_ptr
    {
    public:
        unique_ptr()
        {}

        unique_ptr(T* ptr)
            :m_pObject(ptr)
        {}

        unique_ptr(const unique_ptr& other) = delete;
        unique_ptr& operator=(const unique_ptr& other) = delete;

        unique_ptr(unique_ptr&& other) noexcept
        {
            m_pObject = other.m_pObject;
            other.m_pObject = nullptr;
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept
        {
            if (this == &other)
                return *this;

            m_pObject = other.m_pObject;
            other.m_pObject = nullptr;
            return *this;
        }

        unique_ptr operator=(nullptr_t)
        {
            reset();
        }

        ~unique_ptr()
        {
            if (m_pObject)
            {
                reset();
            }
        }

        T* operator->()
        {
            return m_pObject;
        }

        operator bool() const
        {
            return m_pObject != nullptr;
        }

        void reset()
        {
            if (m_pObject)
            {
                delete m_pObject;
                m_pObject = nullptr;
            }
        }

        T* get() noexcept
        {
            return m_pObject;
        }

    private:
        T* m_pObject = nullptr;
    };

    template <typename T, typename... Args>
    unique_ptr<T> make_unique(Args... args)
    {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }
}

#endif // ASTD_MEMORY