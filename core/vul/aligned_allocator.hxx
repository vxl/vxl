// This is core/vul/aligned_allocator.hxx

#ifndef aligned_allocator_h_
#define aligned_allocator_h_

#include "aligned_memory.hxx"

//:
// \file
// \brief A portable alignment-respecting allocator implementation for SIMD support
// \author Nathan Harbison
// \date October 3, 2025
//
// \endverbatim

// ---------------------------------------------------------------------------------------------
// Allocator implementation

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
inline bool operator== (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept { return true; }

template <typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool operator!= (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept { return false; }

// ---------------------------------------------------------------------------------------------

// Alignment-respecting types related to SIMD

template <typename T>
using aligned_vector = std::vector<T, AlignedAllocator<T, SIMD_ALIGN>>;

#endif // aligned_allocator_h_