// This is core/vul/aligned_memory.h

#ifndef aligned_memory_h_
#define aligned_memory_h_

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <memory>
#ifdef _MSC_VER
    #include <malloc.h>
#endif
#ifdef __SSE__
    #include <immintrin.h>
#endif

//:
// \file
// \brief Portable alignment-respecting memory allocation functions.
// \author Nathan Harbison
// \date October 3, 2025
//
// \endverbatim

//: Rounds down by `align`, given that it is a power of 2
template <typename T>
inline T ALIGN_DOWN(T x, T align) {
  assert(align != 0 && (align & (align - 1)) == 0); // must be power of 2
  return x & ~(align - 1);
}

//: Rounds up by `align`, given that it is a power of 2
template <typename T>
inline T ALIGN_UP(T x, T align) {
  assert(align != 0 && (align & (align - 1)) == 0); // must be power of 2
  return (x + align - 1) & ~(align - 1);
}

// ---------------------------------------------------------------------------------------------
// Allocation functions

//: Allocates the given number of bytes aligned to an address divisible
// by the given alignment. The alignment given must be a power of 2 
// no less than sizeof(void*).
// Throws std::bad_alloc on error
inline void* alloc_aligned_mem(size_t size, size_t alignment) {
    if(alignment < sizeof(void*) || (alignment & (alignment - 1)) != 0)
        throw std::bad_alloc();
#if defined(__SSE__)
    void* ptr = _mm_malloc(size, alignment);
    if(!ptr)
        throw std::bad_alloc();
    return ptr;
#elif defined(_ISOC11_SOURCE) || __cplusplus >= 201703L
    void* ptr = aligned_alloc(alignment, size);
    if(!ptr)
        throw std::bad_alloc();
    return ptr;
#elif defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
    void* ptr;
    if(posix_memalign(&ptr, alignment, size))
        throw std::bad_alloc();
    return ptr;
#elif defined(_MSC_VER)
    void* ptr = _aligned_malloc(size, alignment);
    if(!ptr)
        throw std::bad_alloc();
    return ptr;
#else
    // Manual implementation
    // We'll offset the returned allocation to have the 
    // requested alignment and store this offset before the allocation, 
    // so that we can determine the right pointer to pass to free 
    // when deallocating.
    constexpr size_t max = std::numeric_limits<size_t>::max();
    if(size > max - alignment || size + alignment > max - sizeof(uintptr_t)) // Check for size overflow
        throw std::bad_alloc();
    uintptr_t unaligned = reinterpret_cast<uintptr_t>(malloc(size + alignment + sizeof(uintptr_t)));
    if(!unaligned)
        throw std::bad_alloc();
    uintptr_t aligned = ALIGN_UP(unaligned + sizeof(uintptr_t), alignment);
    uintptr_t* offset_ptr = reinterpret_cast<uintptr_t*>(aligned - sizeof(uintptr_t));
    *offset_ptr = aligned - unaligned;
    return reinterpret_cast<void*>(aligned);
#endif
}

//: Frees an aligned memory allocation
inline void free_aligned_mem(void* p) noexcept { 
    if(!p)
        return;
#if defined(__SSE__)
    _mm_free(p);
#elif (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || defined(_ISOC11_SOURCE) || __cplusplus >= 201703L
    free(p);
#elif defined(_MSC_VER)
    _aligned_free(p);
#else  
    // Manual implementation
    uintptr_t aligned_ptr = reinterpret_cast<uintptr_t>(p);
    uintptr_t* offset_ptr = reinterpret_cast<uintptr_t*>(aligned_ptr - sizeof(uintptr_t));
    free(reinterpret_cast<void*>(aligned_ptr - *offset_ptr)); 
#endif
}

// ---------------------------------------------------------------------------------------------

enum Alignment : size_t {
    DEFAULT = sizeof(void*),
    SSE     = 16,
    AVX     = 32,
    AVX512  = 64
};

// Metadata related to alignment for SIMD types
#define SIMD_ALIGN Alignment::AVX512 // TODO: automatically determined using macros based on SIMD availability
#define NUM_PER_SIMD_VEC(T) ((SIMD_ALIGN == Alignment::DEFAULT) ? 1 : static_cast<size_t>(SIMD_ALIGN) / sizeof(T))

#endif // aligned_memory_h_