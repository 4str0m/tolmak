#pragma once

#include <assert.h>

// #define ARRAY_BOUND_CHECK

template<typename T>
struct Array
{
    inline const T& operator[](int i) const
    {
#ifdef ARRAY_BOUND_CHECK
        assert(i >= 0 && i < count);
#endif
        return data[i];
    }
    inline T& operator[](int i)
    {
#ifdef ARRAY_BOUND_CHECK
        assert(i >= 0 && i < count);
#endif
        return data[i];
    }

    inline void append(const T& t)
    {
        if (count >= allocated)
        {
            reserve(allocated<<1);
            allocated <<= 1;
        }
    }

    inline void reserve(uint32_t count)
    {
        data = reallocarray(data, count, sizeof(T));
    }

private:
    T* data = nullptr;
    uint32_t count = 0;
    uint32_t allocated = 0;
};