#ifndef mbl_functions_h_
#define mbl_functions_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief Collection of common functional objects for vcl_algorithm routines
// \author iscott
// \date  Dec 2001
// Actually, this is mostly an opportunity to mess around in STL to produce code
// which would be much simpler in ordinary C++. Stroustrup assures us that
// this approach is faster in general - which I don't really believe.

//: Produce incrementing values.
// For use with vcl_generate ,e.g.
// \verbatim
//  vcl_generate(A.begin(), A.end(), mbl_increments<unsigned>(0u));
// \endverbatim
template<class T>
struct mbl_increments {
  T val_;
  //: Create an incrementer starting val
  explicit mbl_increments(const T& val=T(0)): val_(val) {}
  T operator()() {return val_++;}
};

//: Produce values by repeatedly apply a funtion to a value.
// For use with vcl_generate, this is a generalisation of mbl_increments
// e.g. the following is equivalent to using mbl_increments
// \verbatim
//  vcl_generate(A.begin(), A.end(),
//               mbl_repeats<unsigned, vcl_binder2nd<vcl_plus<unsigned> > >
//                 (0u, vcl_bind2nd(vcl_plus<unsigned>(), 1u) )
// \endverbatim
template<class T, class UnOp>
struct mbl_repeats {
  mbl_repeats(const T& val, const UnOp& op): val_(val), op_(op) {}
  T val_;
  T operator()() {const T old_val = val_; val_ = op_.operator()(val_); return old_val;}
protected:
  UnOp op_;
};


#endif // mbl_functions_h_

