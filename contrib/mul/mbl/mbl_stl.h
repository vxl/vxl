// This is mul/mbl/mbl_stl.h
#ifndef mbl_stl_h_
#define mbl_stl_h_
//:
// \file
// \brief Useful things missing from vcl_algorithm, etc.
// \author iscott
// \date  Dec 2001
// Actually, this is mostly an opportunity to mess around in STL to produce code
// which would be much simpler in ordinary C++. Stroustrup assures us that
// this approach is faster in general - which I don't really believe.
//
// \verbatim
//  Modifications
//   30 April 2004 - Martin Roberts -
//    Added quite a few little functors mainly to do with iterating through maps
//    for example a version of the non-standard select1st and select2nd

#include <iostream>
#include <functional>
#include <vector>
#include <ostream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Fill an output sequence with incrementing values.
// A bit like std::fill, but after each assignment, the value is incremented.
// \return the next value in the sequence.
template<class Out, class T>
inline T mbl_stl_increments(Out first, Out last, T init)
{
  for (; first != last; ++first, ++init) *first = init;
  return init;
}

//: Fill the first n values of an output sequence with incrementing values.
// A bit like std::fill_n, but after each assignment,
// the value is incremented.
// \return the next value in the sequence.
template<class Out, class Size, class T>
inline T mbl_stl_increments_n(Out first, Size n, T init)
{
  for (; 0 < n; ++first, --n, ++init) *first = init;
  return init;
}

//: Produces a first order sequence from the supplied unary function.
// The value produced at a given step is a function of the previous value.
// E.g. the following is equivalent to using mbl_stl_increments
// \code
// mbl_stl_sequence(A.begin(), A.end(), std::bind1st(std::plus<unsigned>(), 1u), 0u);
// \endcode
// \return the next value in the sequence.
template<class Out, class T, class UnOp>
inline T mbl_stl_sequence(Out first, Out last, UnOp op, T init)
{
  for (;first != last; ++first, init = op(init)) *first = init;
  return init;
}

//: Produces a first order sequence of size n from the supplied function.
// The value produced at a given step is a function of the previous value.
// E.g. the following is equivalent to using mbl_stl_increments
// \return the next value in the sequence.
template<class Out, class Size, class T, class UnOp>
inline T mbl_stl_sequence_n(Out first, Size n, UnOp op, T init)
{
  for (; 0 < n; ++first, --n, init = op(init)) *first = init;
  return init;
}

//: Clean out a range of pointers
// NB the dereferenced iterator must be a pointer
template<class iterType>
inline void mbl_stl_clean(iterType first, iterType last)
{
  for (; first != last; ++first)
  {
    delete *first;
    *first=nullptr;
  }
}

//: Copy elements in input range for which the supplied predicate is true
//Note bizarely although the STL provides remove_copy if etc etc
//the simple copy_if was dropped from the C++ standard
template<typename InputIterator,
         typename OutputIterator,
         typename Predicate>
    inline  OutputIterator mbl_stl_copy_if(InputIterator begin, InputIterator end,
                                           OutputIterator destBegin,
                                           Predicate pred)
{
  while (begin != end)
  {
    if (pred(*begin))
    {
      *destBegin++ = *begin;
    }
    ++begin;
  }
  return destBegin;
}

//----------------------------------------------------------------------------------------------
//Now some map related functors
//
//: select 1st element of a pair (e.g. for map iterators)
//NB something like this is in the SGI extension to the STL but is not included in the standard VCL
//However this is very useful with map iterators so include it here
template <class Pair>
struct mbl_stl_select1st
{
  inline typename Pair::first_type const & operator()(Pair const & pair) const
  {
    return pair.first;
  }
};

//: select 2nd element of a pair (e.g. for map iterators)
//NB something like this is in the SGI extension to the STL but is not included in the standard VCL
//However this is very useful with map iterators so include it here
template <class Pair>
struct mbl_stl_select2nd
{
  inline typename Pair::second_type const & operator()(Pair const & pair) const
  {
    return pair.second;
  }
};

//Accumulate the second elements of a pair (e.g. for accumulating values through a map)
template <class Pair>
struct mbl_stl_add2nd
{
  inline typename Pair::second_type  operator()(typename Pair::second_type partSum, Pair const & x2 ) const
  {
    return partSum + x2.second;
  }
};


// End of map/pair related functors
//------------------------------------------------------------------------------------
//: Given a vector of things, select an indexed element
//For use in eg STL transform algorithm to extract out required subset of (indexed) objects into a working vector
//e.g. given vector of indices and vector of values, copy out the required subset thus
// \code
// std::vector<T> subset
// subset.reserve(indices.size());
// std::transform(indices.begin(),indices.end(),
//               std::back_inserter(subset),
//               mbl_stl_index_functor(values));
// \endcode
template <class T>
class mbl_stl_index_functor
{
  //This functor copies out  element vec[index]
  //For use in eg STL transform algorithm to extract out required subset of (indexed) points into a working vector
  //No bounds checking is done
 private:
  //:const reference to vector used to store the objects indexed
  const std::vector<T >& vec_;

 public:
  mbl_stl_index_functor(const std::vector<T >& vec): vec_(vec) {}
  inline const T& operator()(unsigned index) const { return vec_[index]; }
};


//------------------------------------------------------------------------------------
//: implementation class for use with mbl_stl_output
template <class Cont>
class mbl_stl_output_t1
{
 public:
  const Cont &c;
  const char *sep;
  mbl_stl_output_t1(const Cont& c, const char * sep): c(c), sep(sep) {}
};

//: implementation function for use with mbl_stl_output
template <class Cont> inline
std::ostream& operator<<(std::ostream& s, const mbl_stl_output_t1<Cont>& t)
{
  if (t.c.empty()) return s;
  typename Cont::const_iterator it=t.c.begin(), end=t.c.end();
  s << *it;
  ++it;
  for (; it!=end; ++it)
    s << t.sep << *it;
  return s;
}

//: Allow easy stream output of STL container contents.
// \verbatim
// std::vector<int> c;
// ...
// std::cout << "The contents of c using normal << notation" <<
//   mbl_stl_output(c) << std::endl;
// \endverbatim
template <class Cont> inline
mbl_stl_output_t1<Cont> mbl_stl_output(const Cont &c, const char * sep=" ")
{
  return mbl_stl_output_t1<Cont>(c, sep);
}


//: Find first instance of common value in two sorted sequences.
// \return pair. Either *pair.first == *pair.second, or pair.first == finish1 && pair.second == finish2 if
// no matches are found.
template <class IT1, class IT2>
inline std::pair<IT1, IT2>
  mbl_stl_find_common_value(IT1 start1, IT1 finish1, IT2 start2, IT2 finish2)
{
  std::pair<IT1, IT2> its(start1, start2);
  while (true)
  {
    if (its.first == finish1 || its.second == finish2) return make_pair(finish1, finish2);

    else if (*its.first < *its.second)
      ++its.first;
    else if (*its.second < *its.first)
      ++its.second;
    else return its;
  }
}


//: Find first instance of common value in two sequences sorted by specified comparator.
// \return pair. Either *pair.first == *pair.second, or pair.first == finish1 && pair.second == finish2 if
// no matches are found.
template <class IT1, class IT2, class CMP>
inline std::pair<IT1, IT2>
  mbl_stl_find_common_value(IT1 start1, IT1 finish1, IT2 start2, IT2 finish2, CMP comp = CMP())
{
  std::pair<IT1, IT2> its(start1, start2);
  while (true)
  {
    if (its.first == finish1 || its.second == finish2) return make_pair(finish1, finish2);

    else if (comp(*its.first, *its.second))
      ++its.first;
    else if (comp(*its.second, *its.first))
      ++its.second;
    else return its;
  }
}

#endif // mbl_stl_h_
