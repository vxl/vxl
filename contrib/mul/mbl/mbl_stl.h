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

//: Fill an output sequence with incrementing values.
// A bit like vcl_fill, but after each assignment, the value is incremented.
// \return the next value in the sequence.
template<class Out, class T>
inline T mbl_stl_increments(Out first, Out last, T init)
{
  for (; first != last; ++first, ++init) *first = init;
  return init;
}

//: Fill the first n values of an output sequence with incrementing values.
// A bit like vcl_fill_n, but after each assignment,
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
// mbl_stl_sequence(A.begin(), A.end(), vcl_bind1st(vcl_plus<unsigned>(), 1u), 0u);
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

#endif // mbl_functions_h_
