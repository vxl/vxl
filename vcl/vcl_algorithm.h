#ifndef vcl_algorithm_h_
#define vcl_algorithm_h_

#include <vcl/vcl_compiler.h>

// Each compiler should define this appropriately, so that all
// the #define vcl_blah std::blah can be done in a central place.
// If you get errors later about vcl_algorithm_h_std(blah) it may
// be because someone has #undef'ed vcl_algorithm_h_std. So don't
// do that -- it must be available at the point of expansion of
// vcl_blah.
// --fsm
//#define vcl_algorithm_h_std(x) std::x

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_algorithm.h>
# define vcl_algorithm_h_std(x) /*std::*/::x

#elif defined(VCL_GCC)
# include <algo.h>
# define vcl_algorithm_h_std(x) /*std::*/::x

// eg. sunpro, win32
#else
# include <algorithm>
# include <functional>
# define vcl_algorithm_h_std(x) std::x
#endif

# if defined(VCL_SUNPRO_CC) || defined(VCL_WIN32)
#  ifdef VCL_USE_NATIVE_STL // because this is already in emulation/vcl_algobase.h
// SunPro 5.0 <algorithm> does not supply destroy() - sigh...
// vc 6.0 supplies allocator<T>::destroy(T *) - groan...
template <class T> inline void destroy(T *p) { p->~T(); }
#  endif
# endif

// Enforce definition of vcl_algorithm_h_std
#if !defined(vcl_algorithm_h_std)
  error "please define vcl_algorithm_h_std" // #error not fatal on SunPro 5.0
#endif

// Now #define vcl_blah to std::blah
#define vcl_destroy           destroy
#define vcl_adjacent_find     vcl_algorithm_h_std(adjacent_find)
#define vcl_and 	      vcl_algorithm_h_std(and)
#define vcl_binary 	      vcl_algorithm_h_std(binary)
#define vcl_binary_search     vcl_algorithm_h_std(binary_search)
#define vcl_copy 	      vcl_algorithm_h_std(copy)
#define vcl_copy_ 	      vcl_algorithm_h_std(copy_)
#define vcl_count 	      vcl_algorithm_h_std(count)
#define vcl_count_if 	      vcl_algorithm_h_std(count_if)
#define vcl_equal 	      vcl_algorithm_h_std(equal)
#define vcl_equal_range       vcl_algorithm_h_std(equal_range)
#define vcl_fill 	      vcl_algorithm_h_std(fill)
#define vcl_fill_n 	      vcl_algorithm_h_std(fill_n)
#define vcl_find 	      vcl_algorithm_h_std(find)
#define vcl_find_end 	      vcl_algorithm_h_std(find_end)
#define vcl_find_first_of     vcl_algorithm_h_std(find_first_of)
#define vcl_find_if 	      vcl_algorithm_h_std(find_if)
#define vcl_for_each 	      vcl_algorithm_h_std(for_each)
#define vcl_generate 	      vcl_algorithm_h_std(generate)
#define vcl_generate_n 	      vcl_algorithm_h_std(generate_n)
#define vcl_generators_       vcl_algorithm_h_std(generators_)
#define vcl_heap 	      vcl_algorithm_h_std(heap)
#define vcl_includes 	      vcl_algorithm_h_std(includes)
#define vcl_inplace_merge     vcl_algorithm_h_std(inplace_merge)
#define vcl_iter_swap 	      vcl_algorithm_h_std(iter_swap)
#define vcl_lexicographical_compare vcl_algorithm_h_std(lexicographical_compare)
#define vcl_lower_bound       vcl_algorithm_h_std(lower_bound)
#define vcl_make_heap 	      vcl_algorithm_h_std(make_heap)
#ifndef WIN32
#define vcl_max 	      vcl_algorithm_h_std(max)
#define vcl_min 	      vcl_algorithm_h_std(min)
#endif
#define vcl_max_element       vcl_algorithm_h_std(max_element)
#define vcl_merge 	      vcl_algorithm_h_std(merge)
#define vcl_merge_ 	      vcl_algorithm_h_std(merge_)
#define vcl_min_element       vcl_algorithm_h_std(min_element)
#define vcl_mismatch 	      vcl_algorithm_h_std(mismatch)
#define vcl_next_permutation  vcl_algorithm_h_std(next_permutation)
#define vcl_nth_element       vcl_algorithm_h_std(nth_element)
#define vcl_partial_sort      vcl_algorithm_h_std(partial_sort)
#define vcl_partial_sort_copy vcl_algorithm_h_std(partial_sort_copy)
#define vcl_partition 	      vcl_algorithm_h_std(partition)
#define vcl_partitions_       vcl_algorithm_h_std(partitions_)
#define vcl_pop_heap 	      vcl_algorithm_h_std(pop_heap)
#define vcl_prev_permutation  vcl_algorithm_h_std(prev_permutation)
#define vcl_push_heap 	      vcl_algorithm_h_std(push_heap)
#define vcl_random_shuffle    vcl_algorithm_h_std(random_shuffle)
#define vcl_remove 	      vcl_algorithm_h_std(remove)
#define vcl_remove_copy       vcl_algorithm_h_std(remove_copy)
#define vcl_remove_copy_if    vcl_algorithm_h_std(remove_copy_if)
#define vcl_remove_if 	      vcl_algorithm_h_std(remove_if)
#define vcl_replace 	      vcl_algorithm_h_std(replace)
#define vcl_replace_copy      vcl_algorithm_h_std(replace_copy)
#define vcl_replace_copy_if   vcl_algorithm_h_std(replace_copy_if)
#define vcl_replace_if 	      vcl_algorithm_h_std(replace_if)
#define vcl_reverse 	      vcl_algorithm_h_std(reverse)
#define vcl_reverse_copy      vcl_algorithm_h_std(reverse_copy)
#define vcl_rotate 	      vcl_algorithm_h_std(rotate)
#define vcl_rotate_copy       vcl_algorithm_h_std(rotate_copy)
#define vcl_search 	      vcl_algorithm_h_std(search)
#define vcl_search_n 	      vcl_algorithm_h_std(search_n)
//kym: conflicts with class set.... #define vcl_set 	      vcl_algorithm_h_std(set)
//fsm: which "class set"?
#define vcl_set_difference    vcl_algorithm_h_std(set_difference)
#define vcl_set_intersection  vcl_algorithm_h_std(set_intersection)
#define vcl_set_symmetric_difference vcl_algorithm_h_std(set_symmetric_difference)
#define vcl_set_union 	      vcl_algorithm_h_std(set_union)
#define vcl_sort 	      vcl_algorithm_h_std(sort)
#define vcl_sort_ 	      vcl_algorithm_h_std(sort_)
#define vcl_sort_heap 	      vcl_algorithm_h_std(sort_heap)
#define vcl_stable_sort       vcl_algorithm_h_std(stable_sort)
#define vcl_swap 	      vcl_algorithm_h_std(swap)
#define vcl_swap_ 	      vcl_algorithm_h_std(swap_)
#define vcl_swap_ranges       vcl_algorithm_h_std(swap_ranges)
#define vcl_transform 	      vcl_algorithm_h_std(transform)
#define vcl_unique 	      vcl_algorithm_h_std(unique)
#define vcl_unique_copy       vcl_algorithm_h_std(unique_copy)
#define vcl_upper_bound       vcl_algorithm_h_std(upper_bound)
#endif
