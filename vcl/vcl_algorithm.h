#ifndef vcl_algorithm_h_
#define vcl_algorithm_h_

#include <vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <emulation/vcl_algorithm.h>
# undef vcl_algorithm_h_STD

// -------------------- gcc with old library
#elif (defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)) || defined(VCL_SGI_CC_720)
# include <algo.h>
# define vcl_algorithm_h_STD ::

// -------------------- iso
#else
# include <iso/vcl_algorithm.h>
#endif


#ifdef VCL_VC60
# undef  vcl_max
# define vcl_max vcl_max
# undef  vcl_min
# define vcl_min vcl_min
template <typename T>
inline T vcl_max(T const& a, T const& b)
{
  return (a > b) ? a : b;
}

template <typename T>
inline T vcl_min(T const& a, T const& b)
{
  return (a < b) ? a : b;
}
#endif


// Now #define vcl_blah to std::blah (except for emulation) :
#if defined(vcl_algorithm_h_STD)
# define vcl_adjacent_find     vcl_algorithm_h_STD adjacent_find
# define vcl_and 	       vcl_algorithm_h_STD and
# define vcl_binary 	       vcl_algorithm_h_STD binary
# define vcl_binary_search     vcl_algorithm_h_STD binary_search
# define vcl_copy 	       vcl_algorithm_h_STD copy
# define vcl_copy_ 	       vcl_algorithm_h_STD copy_
# define vcl_count 	       vcl_algorithm_h_STD count
# define vcl_count_if 	       vcl_algorithm_h_STD count_if
# define vcl_equal 	       vcl_algorithm_h_STD equal
# define vcl_equal_range       vcl_algorithm_h_STD equal_range
# define vcl_fill 	       vcl_algorithm_h_STD fill
# define vcl_fill_n 	       vcl_algorithm_h_STD fill_n
# define vcl_find 	       vcl_algorithm_h_STD find
# define vcl_find_end 	       vcl_algorithm_h_STD find_end
# define vcl_find_first_of     vcl_algorithm_h_STD find_first_of
# define vcl_find_if 	       vcl_algorithm_h_STD find_if
# define vcl_for_each 	       vcl_algorithm_h_STD for_each
# define vcl_generate 	       vcl_algorithm_h_STD generate
# define vcl_generate_n        vcl_algorithm_h_STD generate_n
# define vcl_generators_       vcl_algorithm_h_STD generators_
# define vcl_heap 	       vcl_algorithm_h_STD heap
# define vcl_includes 	       vcl_algorithm_h_STD includes
# define vcl_inplace_merge     vcl_algorithm_h_STD inplace_merge
# define vcl_iter_swap 	       vcl_algorithm_h_STD iter_swap
# define vcl_lexicographical_compare \
                               vcl_algorithm_h_STD lexicographical_compare
# define vcl_lower_bound       vcl_algorithm_h_STD lower_bound
# define vcl_make_heap 	       vcl_algorithm_h_STD make_heap
#ifndef WIN32
# define vcl_max 	       vcl_algorithm_h_STD max
# define vcl_min 	       vcl_algorithm_h_STD min
#endif
# define vcl_max_element       vcl_algorithm_h_STD max_element
# define vcl_merge 	       vcl_algorithm_h_STD merge
# define vcl_merge_ 	       vcl_algorithm_h_STD merge_
# define vcl_min_element       vcl_algorithm_h_STD min_element
# define vcl_mismatch 	       vcl_algorithm_h_STD mismatch
# define vcl_next_permutation  vcl_algorithm_h_STD next_permutation
# define vcl_nth_element       vcl_algorithm_h_STD nth_element
# define vcl_partial_sort      vcl_algorithm_h_STD partial_sort
# define vcl_partial_sort_copy vcl_algorithm_h_STD partial_sort_copy
# define vcl_partition 	       vcl_algorithm_h_STD partition
# define vcl_partitions_       vcl_algorithm_h_STD partitions_
# define vcl_pop_heap 	       vcl_algorithm_h_STD pop_heap
# define vcl_prev_permutation  vcl_algorithm_h_STD prev_permutation
# define vcl_push_heap 	       vcl_algorithm_h_STD push_heap
# define vcl_random_shuffle    vcl_algorithm_h_STD random_shuffle
# define vcl_remove 	       vcl_algorithm_h_STD remove
# define vcl_remove_copy       vcl_algorithm_h_STD remove_copy
# define vcl_remove_copy_if    vcl_algorithm_h_STD remove_copy_if
# define vcl_remove_if 	       vcl_algorithm_h_STD remove_if
# define vcl_replace 	       vcl_algorithm_h_STD replace
# define vcl_replace_copy      vcl_algorithm_h_STD replace_copy
# define vcl_replace_copy_if   vcl_algorithm_h_STD replace_copy_if
# define vcl_replace_if        vcl_algorithm_h_STD replace_if
# define vcl_reverse 	       vcl_algorithm_h_STD reverse
# define vcl_reverse_copy      vcl_algorithm_h_STD reverse_copy
# define vcl_rotate 	       vcl_algorithm_h_STD rotate
# define vcl_rotate_copy       vcl_algorithm_h_STD rotate_copy
# define vcl_search 	       vcl_algorithm_h_STD search
# define vcl_search_n 	       vcl_algorithm_h_STD search_n
# define vcl_set_difference    vcl_algorithm_h_STD set_difference
# define vcl_set_intersection  vcl_algorithm_h_STD set_intersection
# define vcl_set_symmetric_difference \
                               vcl_algorithm_h_STD set_symmetric_difference
# define vcl_set_union 	       vcl_algorithm_h_STD set_union
# define vcl_sort 	       vcl_algorithm_h_STD sort
# define vcl_sort_ 	       vcl_algorithm_h_STD sort_
# define vcl_sort_heap 	       vcl_algorithm_h_STD sort_heap
# define vcl_stable_sort       vcl_algorithm_h_STD stable_sort
# define vcl_swap 	       vcl_algorithm_h_STD swap
# define vcl_swap_ 	       vcl_algorithm_h_STD swap_
# define vcl_swap_ranges       vcl_algorithm_h_STD swap_ranges
# define vcl_transform 	       vcl_algorithm_h_STD transform
# define vcl_unique 	       vcl_algorithm_h_STD unique
# define vcl_unique_copy       vcl_algorithm_h_STD unique_copy
# define vcl_upper_bound       vcl_algorithm_h_STD upper_bound
#endif

#endif // vcl_algorithm_h_
