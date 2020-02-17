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

            Bucket& bucket = *m_buckets[i];

            for (uint32_t j = 0; j < N; ++j)
            {
                if (bucket.used[j])
                    continue;

                bucket.used[j] = true;
                return bucket.elements + j;
            }
        }

        // if we reach this point, it means that all buckets were full
        m_buckets_occupation.push_back(1);
        m_buckets.push_back(new Bucket());
        Bucket& bucket = *m_buckets[m_buckets.size()-1];
        memset(bucket.used, 0, sizeof(bool) * N);
        bucket.used[0] = true;
        return bucket.elements;
    }

    void free(T* element)
    {
        for (uint32_t i = 0; i < m_buckets.size(); ++i)
        {
            Bucket& bucket = *m_buckets[i];

            if (element < bucket.elements || element >= bucket.elements+N)
                continue;

            uint32_t index = (uint32_t)(element - bucket.elements);

            bucket.used[index] = false;
            --m_buckets_occupation[i];
            break;
        }
    }

    struct Iterator
    {
        Iterator(BucketAllocator<T, N> *allocator, uint32_t bucket_i, uint32_t elem_i)
        : m_allocator(allocator), m_bucket_i(bucket_i), m_elem_i(elem_i) {}

        inline bool operator==(const Iterator& other) const
        { return m_bucket_i == other.m_bucket_i && m_elem_i == other.m_elem_i; }
        inline bool operator!=(const Iterator& other) const
        { return !operator==(other); }

        inline Iterator& operator++()
        {
            do
            {
                if (m_elem_i == N-1 && m_bucket_i < m_allocator->m_buckets.size()) {
                    ++m_bucket_i;
                    m_elem_i = 0;
                }
                else
                {
                    ++m_elem_i;
                }
                // if we reach the end or the new element is used
                if (*this == m_allocator->end() || m_allocator->m_buckets[m_bucket_i]->used[m_elem_i])
                    break;
            } while(true);
            return *this;
        }

        T& operator*() { return m_allocator->m_buckets[m_bucket_i]->elements[m_elem_i]; }
        T* operator->() { return &m_allocator->m_buckets[m_bucket_i]->elements[m_elem_i]; }
    private:
        BucketAllocator<T, N> *m_allocator;
        uint32_t m_bucket_i;
        uint32_t m_elem_i;
    };

    Iterator begin() { return Iterator(this, 0, 0); }
    Iterator end() { return Iterator(this, m_buckets.size(), 0); }

private:
    struct Bucket
    {
        T elements[N];
        bool used[N];
    };

    Array<Bucket*> m_buckets;
    Array<uint32_t> m_buckets_occupation;
};