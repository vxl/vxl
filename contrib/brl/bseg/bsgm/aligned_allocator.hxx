// This is brl/bseg/bsgm/aligned_allocator.hxx
// TODO: move to location of generic SIMD header / use it
#ifndef aligned_allocator_h_
#define aligned_allocator_h_

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cerrno>
#include <memory>
#ifdef _MSC_VER
    #include <malloc.h>
#endif
#ifdef __SSE__
    #include <immintrin.h>
#endif

//:
// \file
// \brief A portable alignment-respecting allocator implementation for SIMD support
// \author Nathan Harbison
// \date October 3, 2025
//
// \endverbatim

// Alignment helpers, rounds up or down by `align`, given that it is a power of 2
template <typename T>
inline T ALIGN_DOWN(T x, T align) {
  assert(align != 0 && (align & (align - 1)) == 0); // must be power of 2
  return x & ~(align - 1);
}
template <typename T>
inline T ALIGN_UP(T x, T align) {
  assert(align != 0 && (align & (align - 1)) == 0); // must be power of 2
  return (x + align - 1) & ~(align - 1);
}

// ---------------------------------------------------------------------------------------------
// Allocation functions

// Allocates the given number of bytes aligned to an address divisible
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

// Frees an aligned memory allocation
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
// Allocator implementation

enum Alignment : size_t {
    DEFAULT = sizeof(void*),
    SSE     = 16,
    AVX     = 32,
    AVX512  = 64
};

// Allocator class that supports aligned allocations,
// specifically for SIMD support
template <typename T, Alignment Align>
class AlignedAllocator {
private:
    static_assert(
        Align >= alignof(T),
        "Specified alignment is smaller than the required alignment of the type!"
    );
    static_assert(
        Align != 0 && (Align & (Align - 1)) == 0,
        "Specified alignment is not a power of 2!"
    );

public:
    // Types relevant to this class, as specified by the 
    // Allocator requirements (https://en.cppreference.com/w/cpp/named_req/Allocator.html)
    typedef T         value_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef size_t    size_type;

    // Required as AlignedAllocator has a second template
    // argument for the alignment that will make the default
    // std::allocator_traits implementation fail during compilation.
    // See https://stackoverflow.com/a/48062758/2191065
    template<class U>
    struct rebind {
        using other = AlignedAllocator<U, Align>;
    };

    AlignedAllocator() noexcept {}

    template <class U>
    AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept {}

    pointer allocate(size_type n, const_pointer = 0) {
        void* ptr = alloc_aligned_mem(n * sizeof(T), static_cast<size_t>(Align));
        return static_cast<pointer>(ptr);
    }

    void deallocate(pointer p, size_type) noexcept { 
        free_aligned_mem(p);
    }
};

template <typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool
operator== (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept { return true; }

template <typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool operator!= (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept { return true; }

// ---------------------------------------------------------------------------------------------

// Metadata/types related to alignment for SIMD types
#define ALIGN Alignment::AVX512 // TODO: automatically determined using macros based on SIMD availability
#define NUM_ELEMS_PER_ALIGN(T) ((ALIGN == Alignment::DEFAULT) ? 1 : static_cast<size_t>(ALIGN) / sizeof(T))

template <typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T, ALIGN>>;

#endif // aligned_allocator_h_