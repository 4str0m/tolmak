#pragma once

#include <utils/array.h>

template<typename T, int N>
struct BucketAllocator
{
    ~BucketAllocator()
    {
        for (uint32_t i = 0; i < m_buckets.size(); ++i)
            delete m_buckets[i];
    }

    T* allocate()
    {
        for (uint32_t i = 0; i < m_buckets.size(); ++i)
        {
            if (m_buckets_occupation[i] == N)
                continue;

            Bucket& bucket = m_buckets[i];

            for (uint32_t j = 0; j < N; ++j)
            {
                if (bucket.used[j])
                    continue;

                bucket.used[j] = true;
                return bucket.elements[j];
            }
        }

        // if we reach this point, it means that all buckets were full
        m_buckets_occupation.append(1);
        m_buckets.append(new Bucket());
        Bucket& bucket = m_buckets[m_buckets.size()-1];
        memset(bucket.used, 0, sizeof(bool) * N)
        bucket.used[0] = true;
        return bucket.elements[0];
    }

    void free(T* element)
    {
        for (uint32_t i = 0; i < m_buckets.size(); ++i)
        {
            Bucket& bucket = m_buckets[i];

            if (element < bucket.elements || element >= bucket.elements+N)
                continue;

            uint32_t index = (uint32_t)(element - bucket.elements);

            bucket.used[index] = false;
            --m_buckets_occupation[i];
            break;
        }
    }

private:
    struct Bucket
    {
        T elements[N];
        bool used[N];
    };

    Array<Bucket*> m_buckets;
    Array<uint32_t> m_buckets_occupation;
};