// This is brl/bseg/bsgm/aligned_allocator.hxx
// TODO: move to location of generic SIMD header / use it
#ifndef aligned_allocator_h_
#define aligned_allocator_h_

#include <cstddef>
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
    // Allocator requirements
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

    // The alignment of allocations made by this allocator, if not set to DEFAULT.
    // Otherwise, the alignment of the allocated type.
    static constexpr size_t alignment = (Align == DEFAULT) ? alignof(T) : static_cast<size_t>(Align);
    // The alignment of allocations made by this allocator
    static constexpr size_t alloc_alignment = static_cast<size_t>(Align);

    AlignedAllocator() noexcept {}

    template <class U>
    AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept {}

    pointer allocate(size_type n, const_pointer = 0) {
#if defined(__SSE__)
        void* ptr = _mm_malloc(n * sizeof(T), alloc_alignment);
        if(!ptr) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<pointer>(ptr);
#elif defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
        void* ptr;
        if(posix_memalign(&ptr, alloc_alignment, n * sizeof(T))) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<pointer>(ptr);
#elif defined(_ISOC11_SOURCE)
        void* ptr = aligned_alloc(alloc_alignment, n * sizeof(T));
        if(!ptr) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<pointer>(ptr);
#elif __cplusplus >= 201703L
        return new (std::align_val_t(Align)) T[n];
#elif defined(_MSC_VER)
        void* ptr = _aligned_malloc(n * sizeof(T), alloc_alignment);
        if(!ptr) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<pointer>(ptr);
#else
    #error "Could not find an alignment-satisfying allocation function. Requires C++17 or above, SSE, POSIX.1-2001, ISO C11, or MSVC."    
#endif
    }

    void deallocate(pointer p, size_type) noexcept { 
#if defined(__SSE__)
        _mm_free(p);
#elif (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L) || defined(_ISOC11_SOURCE)
        std::free(p);
#elif __cplusplus >= 201703L
        delete[] ptr;
#elif defined(_MSC_VER)
        _aligned_free(p);
#else
    #error "Could not find an alignment-satisfying allocation function. Requires C++17 or above, SSE, POSIX.1-2001, ISO C11, or MSVC."    
#endif
    }
};

template <typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool
operator== (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept { return true; }

template <typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool operator!= (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept { return true; }

#endif // aligned_allocator_h_