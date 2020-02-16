#pragma once
#include <cassert>
#include "type_traits.h"
#include "error.h"

/*
TODOS
Constructors from string

to_string
to_ulong
to_ullong

Non Members:
operator&
operator|
operator^
operator<<
operator>>

helpers:
std::hash
*/

namespace
{
    template<typename T>
    struct isValidBaseType : astd::false_type
    {};

    template<>
    struct isValidBaseType<unsigned long> : astd::true_type
    {};

    template<>
    struct isValidBaseType<unsigned long long> : astd::true_type
    {};
}

namespace astd
{
    template<size_t _size>
    class bitset
    {
    private:
#ifdef BITSET_BASE_TYPE
        using base_type = BITSET_BASE_TYPE
#else
        using base_type = unsigned long;
#endif // BITSET_BASE_TYPE
        static_assert(isValidBaseType<base_type>::value, "not a valid base_type for bitset");

    public:
        class reference
        {
            friend bitset<_size>;

        public:
            ~reference() noexcept
            {}

            reference& operator=(bool val) noexcept
            {
                m_pBitset->set_impl(m_idx, val);
                return *this;
            }

            reference& operator=(const reference& ref) noexcept
            {
                m_pBitset->set_impl(m_idx, static_cast<bool>(ref));
                return *this;
            }

            operator bool() const noexcept
            {
                return m_pBitset->subscr_impl(m_idx);
            }

            [[nodiscard]] bool operator~() const noexcept
            {
                return !m_pBitset->subscr_impl(m_idx);
            }

            reference& flip() noexcept
            {
                m_pBitset->flip_impl(m_idx);
                return *this;
            }

        private:
            reference() noexcept
            {}

            reference(bitset<_size>& bitset, size_t idx) noexcept
                :m_pBitset(&bitset), m_idx(idx)
            {}

            bitset<_size>* m_pBitset = nullptr;
            size_t m_idx = 0;
        };

        constexpr bitset() noexcept
            :m_data{}
        {}

        bitset(unsigned long long val) noexcept // TODO can this be made constexpr?
        {
            if constexpr (is_same<base_type, unsigned long long>::value)
                m_data[0] = val;
            else
            {
                constexpr auto sizediff = sizeof(unsigned long long) - sizeof(base_type);
                if constexpr (sizediff > 0 && s_arrSize > 1)
                {
                    m_data[0] = static_cast<base_type>(val);
                    m_data[1] = (val >> sizeof(base_type) * 8) && ~static_cast<base_type>(0);
                }
                else
                    m_data[0] = static_cast<base_type>(val);
            }

            sanatize_trail();
        }

        [[nodiscard]] bool operator==(const bitset<_size>& other) const noexcept
        {
            return memcmp(m_data, other.m_data, sizeof(m_data)) == 0;
        }

        [[nodiscard]] constexpr bool operator[](size_t idx) const
        {
#if _DEBUG
            verify(idx < _size, "bitset subscript out of range");
#endif
            return subscr_impl(idx);
        }

        [[nodiscard]] reference operator[](size_t idx)
        {
#if _DEBUG
            verify(idx < _size, "bitset subscript out of range");
#endif
            return reference(*this, idx);
        }

        [[nodiscard]] bool test(size_t idx) const
        {
            verify(idx < _size, "invalid bitset index");
            return subscr_impl(idx);
        }

        [[nodiscard]] bool all() const noexcept
        {
            return count() == _size; //TODO this can be done faster
        }

        [[nodiscard]] bool any() const noexcept
        {
            for (size_t dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                if (m_data[dataIdx] != static_cast<base_type>(0))
                    return true;

            return false;
        }

        [[nodiscard]] bool none() const noexcept
        {
            return !any();
        }

        [[nodiscard]] size_t count() const noexcept
        {
            size_t result = 0;
            for (size_t dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                if constexpr (is_same<base_type, unsigned long>::value)
                    result += __builtin_popcountl(m_data[dataIdx]);
                else if constexpr (is_same<base_type, unsigned long long>::value)
                    result += __builtin_popcountll(m_data[dataIdx]);
            return result;
        }

        [[nodiscard]] constexpr size_t size() const noexcept
        {
            return _size;
        }

        bitset& operator&=(const bitset& other) noexcept
        {
            for (size_t dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                m_data[dataIdx] &= other.m_data[dataIdx];

            return *this;
        }

        bitset& operator|=(const bitset& other) noexcept
        {
            for (size_t dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                m_data[dataIdx] |= other.m_data[dataIdx];

            return *this;
        }

        bitset& operator^=(const bitset& other) noexcept
        {
            for (size_t dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                m_data[dataIdx] ^= other.m_data[dataIdx];

            return *this;
        }

        bitset& operator~() const
        {
            return bitset(*this).flip();
        }

        bitset& operator<<(size_t offset) const noexcept
        {
            return bitset(*this) <<= offset;
        }

        bitset& operator<<=(size_t offset) noexcept
        {
            // Check if the shift is bigger than length of base type
            // if so, shift the whole data elements accordingly
            const auto dataIdxShift = static_cast<int>(offset / s_bitsInBaseType);

            if (dataIdxShift != 0) // > 0
            {
                for (int dataIdx = s_arrSize - 1; dataIdx >= 0; --dataIdx)
                    m_data[dataIdx] = dataIdx - dataIdxShift >= 0 ? m_data[dataIdx - dataIdxShift] : static_cast<base_type>(0);
            }

            // then do bitshifting on rest of the elems
            // taking into account the shifted off bits
            if (offset % s_bitsInBaseType != 0)
            {
                for (int dataIdx = s_arrSize - 1; dataIdx >= 0; --dataIdx)
                    m_data[dataIdx] = (m_data[dataIdx] << (offset % s_bitsInBaseType)) | (dataIdx > 0 ? m_data[dataIdx - 1] >> (s_bitsInBaseType - (offset % s_bitsInBaseType)) : static_cast<base_type>(0));
            }

            sanatize_trail();

            return *this;
        }

        bitset& operator>>(size_t offset) const noexcept
        {
            return bitset(*this) >>= offset;
        }

        bitset& operator>>=(size_t offset) noexcept
        {
            // Check if the shift is bigger than length of base type
            // if so, shift the whole data elements accordingly
            const auto dataIdxShift = static_cast<int>(offset / s_bitsInBaseType);

            if (dataIdxShift != 0) // > 0
            {
                for (int dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                    m_data[dataIdx] = dataIdx + dataIdxShift < s_arrSize ? m_data[dataIdx + dataIdxShift] : static_cast<base_type>(0);
            }

            // then do bitshifting on rest of the elems
            // taking into account the shifted off bits
            if (offset % s_bitsInBaseType != 0)
            {
                for (int dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                    m_data[dataIdx] = (m_data[dataIdx] >> (offset % s_bitsInBaseType)) | (dataIdx < s_arrSize - 1 ? m_data[dataIdx + 1] << (s_bitsInBaseType - (offset % s_bitsInBaseType)) : static_cast<base_type>(0));
            }

            return *this;
        }

        bitset& set() noexcept
        {
            memset(m_data, 0xFF, sizeof(m_data) * s_arrSize);
            sanatize_trail();
            return *this;
        }

        bitset& set(size_t idx, bool value = true)
        {
            verify(idx < _size, "invalid bitset index")
            set_impl(idx, value);
            return *this;
        }

        bitset& reset() noexcept
        {
            memset(m_data, 0x00, sizeof(m_data) * s_arrSize);
            return *this;
        }

        bitset& reset(size_t idx)
        {
            verify(idx < _size, "invalid bitset index"
            set_impl(idx, false);
            return *this;
        }

        bitset& flip() noexcept
        {
            for (size_t dataIdx = 0; dataIdx < s_arrSize; ++dataIdx)
                m_data[dataIdx] = ~m_data[dataIdx];

            sanatize_trail();
        }

        bitset& flip(size_t idx)
        {
            verify(idx < _size, "invalid bitset index"
            flip_impl(idx);
            return *this;
        }

    private:
        constexpr bool subscr_impl(size_t idx) const noexcept
        {
            return (m_data[idx / s_bitsInBaseType] & (static_cast<base_type>(1) << idx % s_bitsInBaseType)) != static_cast<base_type>(0);
        }
        constexpr void set_impl(size_t idx, bool val) noexcept
        {
            const auto bitmask = static_cast<base_type>(1) << idx % s_bitsInBaseType;
            if (val)
                m_data[idx / s_bitsInBaseType] |= bitmask;
            else
                m_data[idx / s_bitsInBaseType] &= ~bitmask;
        }
        constexpr void flip_impl(size_t idx) noexcept
        {
            m_data[idx / s_bitsInBaseType] ^= static_cast<base_type>(1) << idx % s_bitsInBaseType;
        }

        void sanatize_trail() noexcept
        {
            if constexpr (_size % s_bitsInBaseType != 0)
                m_data[s_arrSize - 1] &= (1 << _size % s_bitsInBaseType) - 1;
        }

        static constexpr size_t s_bitsInBaseType = 8 * sizeof(base_type);
        static constexpr size_t s_arrSize = _size == 0 ? 0 : (_size - 1) / s_bitsInBaseType + 1;

        base_type m_data[s_arrSize];
    };
}