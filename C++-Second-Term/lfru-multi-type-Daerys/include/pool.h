#pragma once

#include <cstddef>
#include <initializer_list>
#include <new>
#include <vector>

class PoolAllocator
{
public:
    PoolAllocator(const std::size_t block_size, std::initializer_list<std::size_t> sizes)
        : m_block_size(block_size)
        , m_block_count(sizes.size())
        , m_blocks(m_block_count)
        , m_data(m_block_size * m_block_count)
        , m_sizes(sizes)
    {
        for (std::size_t i = 0; i < sizes.size(); i++) {
            m_blocks[i].resize(m_block_size / m_sizes[i]);
        }
    };
    void * allocate(const std::size_t n);

    void deallocate(const void * ptr);

private:
    const std::size_t m_block_size;
    const std::size_t m_block_count;
    std::vector<std::vector<bool>> m_blocks;
    std::vector<std::byte> m_data;
    std::vector<std::size_t> m_sizes;
};
