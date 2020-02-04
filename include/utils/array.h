#pragma once

#include <utils/log.h>

#define ARRAY_BOUND_CHECK 0

template<typename T>
struct Array
{
    Array()
    {
        reserve(16);
    }
    ~Array()
    {
        free(m_data);
        m_data = nullptr;
        m_size = m_allocated = 0;
    }


    inline const T& operator[](int i) const
    {
#if ARRAY_BOUND_CHECK == 1
        ASSERT(i >= 0 && i < m_size, "array index out of bounds (%d not in [0, %u]).", i, m_size);
#endif
        return m_data[i];
    }
    inline T& operator[](int i)
    {
#if ARRAY_BOUND_CHECK == 1
        ASSERT(i >= 0 && i < m_size, "array index out of bounds (%d not in [0, %u]).", i, m_size);
#endif
        return m_data[i];
    }

    inline void append(const T& t)
    {
        if (m_size >= m_allocated)
            reserve(m_allocated<<1);

        m_data[m_size] = t;
        ++m_size;
    }

    inline void reserve(uint32_t size)
    {
        T* new_data = (T*)reallocarray(m_data, size, sizeof(T));
        if (!new_data)
            LOG(ERROR, "not able to reserve %u elements for array.", size);
        m_data = new_data;
        m_allocated = size;
    }

    inline bool empty() const { return m_size == 0; }
    inline void clear() { m_size = 0; }

    inline T pop()
    {
#if ARRAY_BOUND_CHECK == 1
        ASSERT(m_size > 0, "cannot pop element from empty array.");
#endif
        return m_data[--m_size];
    }

    inline uint32_t size() const { return m_size; }
    inline const T* data() const { return m_data; }

private:
    T* m_data = nullptr;
    uint32_t m_size = 0;
    uint32_t m_allocated = 0;
};