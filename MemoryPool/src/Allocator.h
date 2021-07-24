#pragma once
#include <vector>

/// <summary>
/// A node that contains pointer to memory, allocated size and pointers to neighboring nodes
/// </summary>
struct Block
  {
  Block* next = nullptr;
  Block* prev = nullptr;
  size_t  used_mem_size = 0;
  size_t  free_mem_size = 0;
  int8_t* p_mem_chunk = nullptr;
  };


class Allocator
  {
    int8_t*            mp_pool;
    bool               m_is_valid_pool;
    size_t             m_pool_size;
    std::vector<Block> m_blocks;

    Block* add_block_to_buckets();
    
    public:

    explicit Allocator(size_t pool_size) noexcept;
    Allocator(const Allocator&) = delete;
    Allocator(Allocator&&) = delete;
    Allocator& operator= (const Allocator&) = delete;
    Allocator& operator= (Allocator&&) = delete;
    ~Allocator();

    void* allocate(size_t size_to_alloc);

    template <typename T>
    void deallocate(T* p_mem_to_dealloc);
  };


Allocator::Allocator(size_t pool_size) noexcept
  : mp_pool{ nullptr },  m_is_valid_pool{ false },  m_pool_size{ pool_size }
  {
  mp_pool = reinterpret_cast<int8_t*>(malloc(pool_size));

  std::cout << " >>> Creating memory pool ... ";

  if (mp_pool)
    {
    m_is_valid_pool = true;

    size_t reserv_first_last_dummy = 3;
    m_blocks.reserve(pool_size + reserv_first_last_dummy); // reserving memory for blocks 
    /* TODO problem with reserving a lot of memory for blocks (approx 20 bytes per one byte of memory chunk in pool)
     * One way to solve the problem is to set the minimal size of allocated memory chunk */

    // creating first block that points to the begin of allocated memory
    m_blocks.emplace_back();
    m_blocks[0].p_mem_chunk = mp_pool;
    m_blocks[0].free_mem_size = pool_size;

    // creating last block that represents the end of allocated memory
    m_blocks.emplace_back();
    m_blocks[1].p_mem_chunk = nullptr;
    m_blocks[1].free_mem_size = 0;

    // connecting first and last buckets
    m_blocks[0].next = &m_blocks[1];
    m_blocks[1].prev = &m_blocks[0];

    std::cout << " OK" << std::endl;
    }
  else
    {
    std::cerr << " FAILED" << std::endl;
    }

  std::cout << " >>> Allocated [" << m_pool_size << "] bytes " << std::endl;
  }

Allocator::~Allocator()
  {
  free(mp_pool);
  }

/// <summary>
/// This function allocates memory chunk from the pool
/// </summary>
/// <param name="size_to_alloc"> - size of memory that need to be allocated, bytes </param>
/// <returns> pointer to memory chunk</returns>
inline void* Allocator::allocate(size_t size_to_alloc)
  {
  if (!m_is_valid_pool || size_to_alloc < 1)
    {
    return nullptr;
    }

  /* looking for free memory chunk */
  Block* cur_block = &(m_blocks[0]);
  while (cur_block->next != nullptr)
    {
    if (cur_block->free_mem_size >= size_to_alloc)
      {
      break;
      }
    cur_block = cur_block->next;
    }
  if (cur_block->next == nullptr)
    {
    return nullptr;
    }

  /* if there is free memory chunk still available after allocation
   * the new block that points to this chunk is created */
  if (cur_block->free_mem_size - size_to_alloc > 0)                                 
    {                                               
    Block* block_to_insert = add_block_to_buckets();
    block_to_insert->p_mem_chunk = cur_block->p_mem_chunk + size_to_alloc;
    block_to_insert->free_mem_size = cur_block->free_mem_size - size_to_alloc;

    cur_block->free_mem_size = 0;
    cur_block->used_mem_size = size_to_alloc;

    block_to_insert->next = cur_block->next;
    cur_block->next->prev = block_to_insert;
    block_to_insert->prev = cur_block;
    cur_block->next = block_to_insert;
    }
  /* if no memory is available after allocation, just allocating whole chunk */
  else 
    {
    cur_block->free_mem_size = 0;
    cur_block->used_mem_size = size_to_alloc;
    }

  return cur_block->p_mem_chunk;
  }

/// <summary>
/// This function deallocates memory chunk from the pool
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="p_mem_to_dealloc"> - pointer to previously allocated memory</param>
template <typename T>
inline void Allocator::deallocate(T* p_mem_to_dealloc)
  {
  if (!m_is_valid_pool || p_mem_to_dealloc == nullptr)
    {
    std::cout << " >>> Deallocation FAILED !!!" << std::endl;
    return;
    }

  /* looking if provided pointer is really from this memory pool */
  Block* cur_block = &(m_blocks[0]);
  while (cur_block->next != nullptr)
    {
    if (cur_block->p_mem_chunk == (int8_t*)p_mem_to_dealloc)
      {
      break;
      }
    cur_block = cur_block->next;
    }
  if (cur_block->next == nullptr)
    {
    std::cout << " >>> Deallocation FAILED !!! No memory was allocated for provided pointer !!!" << std::endl;
    return;
    }

  /* freeing memory */
  cur_block->free_mem_size = cur_block->used_mem_size;
  cur_block->used_mem_size = 0;
  // TODO: problem with garbage data left in memory chunk
  
  /* if previous block contains free memory, unite */
  if (cur_block->prev != nullptr && cur_block->prev->used_mem_size == 0)
    {
    cur_block->prev->next = cur_block->next;
    cur_block->next->prev = cur_block->prev->next;
    cur_block->prev->free_mem_size += cur_block->free_mem_size;
    Block* temp = cur_block;
    cur_block = cur_block->prev;

    temp->next = nullptr;
    temp->prev = nullptr;
    temp->p_mem_chunk = nullptr;
    }

  /* if next block contains free memory, unite */
  if (cur_block->next->next != nullptr && cur_block->next->used_mem_size == 0 )
    {
    cur_block->free_mem_size += cur_block->next->free_mem_size;
    cur_block->next->next->prev = cur_block;
    Block* temp = cur_block->next;
    cur_block->next = cur_block->next->next;

    temp->next = nullptr;
    temp->prev = nullptr;
    temp->p_mem_chunk = nullptr;
    }  
  }


/// <summary>
/// This function allows for adding new block in O(N) without allocating new memory for it
/// because of reserved memory in std::vector<Block> m_blocks
/// </summary>
/// <returns></returns>
inline Block* Allocator::add_block_to_buckets()
  {
  m_blocks.emplace_back();

  for (size_t i = 2; i < m_blocks.size()-1 ; ++i)
    {
    if (m_blocks[i].next == nullptr && m_blocks[i].prev == nullptr)
      {
      m_blocks[i] = std::move(m_blocks.back());
      m_blocks.pop_back();
      return &m_blocks[i];
      }
    }

  return &m_blocks.back();
  }

