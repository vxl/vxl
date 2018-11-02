// This is mul/mbl/mbl_mask.h
#ifndef mbl_mask_h_
#define mbl_mask_h_
//:
// \file
// \author Barry Skellern
// \brief Class representing a binary mask, and related functions

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <iterator>
#include <string>
#include <iosfwd>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_sprintf.h>


//: Defines a binary mask
// \note include vsl/vsl_vector_io.h in your client code, if your want binary io of mbl_mask
class mbl_mask : public std::vector<bool>
{
 public:
  mbl_mask() : std::vector<bool>() {}
  mbl_mask(unsigned n, bool val = false) : std::vector<bool>(n, val) {}
};

//: Given a collection of indices, produce a collection of masks that isolate each indexed set
//    The input index set does not need to be zero based or continuous
//    The output vector of masks is sorted by corresponding index
//    for example: (1,4,2,1,2) will make three masks: (1,0,0,1,0), (0,0,1,0,1) and (0,1,0,0,0)
//    which correspond to the index sets 1,2,4
void mbl_masks_from_index_set(const std::vector<unsigned> & indices,
                              std::vector<mbl_mask> & masks);


//: Replace 'true' values in B with values taken from A. size of A must match 'true' count in B
void mbl_mask_on_mask(const mbl_mask & A, mbl_mask & B);


//: Merge two input ranges according to a mask ('false' indicates element drawn from range 1, 'true' from range 2)
template <typename ForwardIterator>
void mbl_mask_merge_values(const mbl_mask & mask,
                           ForwardIterator first1, ForwardIterator last1,
                           ForwardIterator first2, ForwardIterator last2,
                           ForwardIterator result)
{
  if (std::distance(first1, last1) != (int)mask.size() || std::distance(first2, last2) != (int)mask.size())
    throw std::out_of_range("Values and mask lengths differ");

  for (unsigned n = 0 ; first1 != last1 ; ++first1, ++first2, ++n)
    *result++ = mask[n] ? *first2 : *first1;
}

//: Apply a general logical operation between two masks
void mbl_mask_logic(const mbl_mask & A, mbl_mask & B, const std::string & operation);

//: Apply an "AND" (rule 0001) logical operation between two masks
void mbl_mask_logic_and(const mbl_mask & A, mbl_mask & B);

//: Apply an "OR" (rule 0111) logical operation between two masks
void mbl_mask_logic_or(const mbl_mask & A, mbl_mask & B);

//: Apply an "XOR" (rule 0110) logical operation between two masks
void mbl_mask_logic_xor(const mbl_mask & A, mbl_mask & B);

//: Apply a "NOR" (rule 1000) logical operation between two masks
void mbl_mask_logic_nor(const mbl_mask & A, mbl_mask & B);

//: Apply an "XNOR" (rule 1001) logical operation between two masks
void mbl_mask_logic_xnor(const mbl_mask & A, mbl_mask & B);

//: Apply an "NAND" (rule 1110) logical operation between two masks
void mbl_mask_logic_nand(const mbl_mask & A, mbl_mask & B);

//: Apply a mask to a range of values. Result inserted at 'target'
template <typename ForwardIterator, typename OutputIterator>
void mbl_apply_mask(const mbl_mask & mask, ForwardIterator first, ForwardIterator last, OutputIterator target)
{
  if (std::distance(first, last) != (int)mask.size())
    throw std::out_of_range("Values and mask lengths differ");

  for (unsigned n = 0; first != last ; ++first, ++n)
    if (mask[n]) *target++ = *first;
}

//: Apply a mask to a vector, returning a new vector
// This can be inefficient. See mbl_apply_mask(mask, src, dst)  for an alternative.
template <typename T>
std::vector<T> mbl_apply_mask(const mbl_mask & mask, const std::vector<T> & values)
{
  std::vector<T> retval(values);
  mbl_apply_mask(mask, retval);
  return retval;
}

//: Apply a mask to a vector, returning a new vector
// \param mask The mask to apply.
// \param src The source vector.
// \retval dst The destination vector (existing contents will be lost).
template <typename T>
void mbl_apply_mask(const mbl_mask & mask, const std::vector<T> & src, std::vector<T> & dst)
{
  const unsigned n_in = src.size();
  if (mask.size() != n_in)
  {
    throw std::out_of_range(vul_sprintf("src and mask lengths differ: src %d mask %d",n_in,mask.size()));
  }

  dst.clear();
  dst.reserve(n_in); // this is the maximum size we might need
  for (unsigned i=0; i<n_in; ++i)
  {
    if (mask[i])
    {
      dst.push_back(src[i]);
    }
  }
}

//: Use a mask to replace some values in a vector
// \param mask The mask to apply.
// \param src1 The source vector to be updated.
// \param src2 The source vector to be updated with.
// \retval dst The destination vector (existing contents will be lost).
template <typename T>
void mbl_replace_using_mask(const mbl_mask & mask, const std::vector<T> & src1, const std::vector<T> & src2, std::vector<T> & dst)
{
  const unsigned n_in = src1.size();
  if (mask.size() != n_in)
    throw std::out_of_range("src1 and mask lengths differ");

  std::size_t n_true = std::count( mask.begin(), mask.end(), true );
  if ( n_true != src2.size() )
    throw std::out_of_range("src2 and mask are not compatible");

  std::vector<T> dst_tmp;
  dst_tmp.clear();
  dst_tmp.reserve(n_in); // this is the maximum size we might need
  unsigned j = 0;
  for (unsigned i=0; i<n_in; ++i)
  {
    if (mask[i])
    {
      dst_tmp.push_back(src2[j]);
      ++j;
    }
    else
      dst_tmp.push_back(src1[i]);
  }
  dst = dst_tmp;
}

//: Apply a mask to a vector in-place
template <typename T>
void mbl_apply_mask(const mbl_mask & mask, std::vector<T> & values)
{
  const unsigned n_in = values.size();
  if (mask.size() != n_in)
    throw std::out_of_range("Values and mask lengths differ");

  unsigned n_out = 0;
  for (unsigned i = 0 ; i < n_in ; ++i)
  {
    if (mask[i])
    {
      values[n_out] = values[i];
      ++n_out;
    }
  }
  values.resize(n_out);
}

//: Save to file
void mbl_save_mask(const mbl_mask & mask, std::ostream & stream);

//: Save to file
void mbl_save_mask(const mbl_mask & mask, const char * filename);

//: Save to file
void mbl_save_mask(const mbl_mask & mask, const std::string &filename);

//: Load from file
void mbl_load_mask(mbl_mask & mask, std::istream & stream);

//: Load from file
void mbl_load_mask(mbl_mask & mask, const char * filename);

//: Load from file
void mbl_load_mask(mbl_mask & mask, const std::string &filename);

//: Convert a mask to a list of indices.
// \param mask Input mask.
// \retval inds List of (zero-based) indices i where mask[i]==true.
void mbl_mask_to_indices(const mbl_mask& mask, std::vector<unsigned>& inds);

//: Convert a list of indices to a mask.
// \param inds List of (zero-based) indices.
// \param n The length of the output mask.
// \retval mask Output mask. mask[i]==true for all i in \a inds
void mbl_indices_to_mask(const std::vector<unsigned>& inds,
                         const unsigned n,
                         mbl_mask& mask);




#endif // mbl_mask_h_
