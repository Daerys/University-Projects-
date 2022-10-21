#pragma once

#include <algorithm>
#include <cstddef>
#include <list>
#include <new>
#include <ostream>

template <class Key, class KeyProvider, class Allocator>
class Cache
{
public:
    template <class... AllocArgs>
    Cache(const std::size_t cache_size, AllocArgs &&... alloc_args)
        : m_max_top_size(cache_size)
        , m_max_low_size(cache_size)
        , m_alloc(std::forward<AllocArgs>(alloc_args)...)
    {
    }

    std::size_t size() const
    {
        return m_lru.size() + m_queue.size();
    }

    bool empty() const
    {
        return m_lru.empty() && m_queue.empty();
    }

    template <class T>
    T & get(const Key & key);

    std::ostream & print(std::ostream & strm) const;

    friend std::ostream & operator<<(std::ostream & strm, const Cache & cache)
    {
        return cache.print(strm);
    }

private:
    const std::size_t m_max_top_size;
    const std::size_t m_max_low_size;
    Allocator m_alloc;
    std::list<KeyProvider *> m_lru;
    std::list<KeyProvider *> m_queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T & Cache<Key, KeyProvider, Allocator>::get(const Key & key)
{
    auto key_comp = [&key](const KeyProvider * ptr) { return *ptr == key; };
    auto lru_pos = std::find_if(m_lru.begin(), m_lru.end(), key_comp);
    auto queue_pos = std::find_if(m_queue.begin(), m_queue.end(), key_comp);
    bool flag = lru_pos != m_lru.end();

    if (!flag && queue_pos == m_queue.end()) {
        if (m_queue.size() == m_max_low_size) {
            m_alloc.template destroy<KeyProvider>(m_queue.front());
            m_queue.erase(m_queue.begin());
        }
        m_queue.push_back(m_alloc.template create<T>(key));
        return *static_cast<T *>(m_queue.back());
    }
    if (!flag && queue_pos != m_queue.end()) {
        if (m_lru.size() == m_max_top_size) {
            m_queue.push_back(m_lru.back());
            m_lru.pop_back();
        }
    }
    m_lru.splice(m_lru.begin(), flag ? m_lru : m_queue, flag ? lru_pos : queue_pos);

    return *static_cast<T *>(m_lru.front());
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream & Cache<Key, KeyProvider, Allocator>::print(std::ostream & strm) const
{
    for (const auto i : m_lru) {
        strm << *i << " ";
    }
    for (const auto i : m_queue) {
        strm << *i << " ";
    }
    return strm << " |booba| ";
}
