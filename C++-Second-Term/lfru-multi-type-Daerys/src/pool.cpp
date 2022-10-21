#include "allocator.h"

#include <algorithm>

void * PoolAllocator::allocate(const std::size_t n)
{
    auto key = std::find(m_sizes.begin(), m_sizes.end(), n);
    auto index = key - m_sizes.begin();
    for (std::size_t j = 0; j < m_blocks[index].size(); j++) {
        if (!m_blocks[index][j]) {
            m_blocks[index][j] = true;
            return &m_data[m_sizes[index] * j + m_block_size * index];
        }
    }
    throw std::bad_alloc{};
}

void PoolAllocator::deallocate(const void * ptr)
{
    const auto pointer = static_cast<const std::byte *>(ptr);
    const std::size_t offset = static_cast<std::size_t>(pointer - &m_data[0]);
    if (offset <= m_block_size * m_block_count) {
        std::size_t k = offset / m_block_size;
        std::size_t j = (offset % m_block_size) / m_sizes[k];
        m_blocks[k][j] = false;
    }
}
