// This is ./vxl/vnl/vnl_finite_field_number.h
#ifndef vnl_finite_field_number_h_
#define vnl_finite_field_number_h_

//:
// \file
// \brief finite field arithmetic
//
// The templated vnl_finite_field_number<N> class provides finite (Galois) field
// numbers and arithmetic.  The template argument is the cardinality of the
// finite field.  This number must be a prime number.  Otherwise, a finite
// ring (or semi-field) is obtained, for which division makes no sense,
// but all other operations remain valid.
//
// \author
//  Peter Vanroose, K.U.Leuven, ESAT/PSI.
// \date 5 May 2002.

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for sqrt

//: finite field arithmetic
//
// The templated vnl_finite_field_number<N> class provides finite (Galois) field
// numbers and arithmetic.  The template argument is the cardinality of the
// finite field.  This number must be a prime number.  Otherwise, a finite
// ring (or semi-field) is obtained, for which division makes no sense,
// but all other operations remain valid.
//
template <int N>
class vnl_finite_field_number {
  typedef vnl_finite_field_number<N> Base;
public:
  //: Creates a finite field element.
  //  Default constructor gives 0.
  //  Also serves as automatic cast from int to vnl_finite_field_number.
  inline vnl_finite_field_number(int x = 0) : val_(x<0?N-1-((-x-1)%N):x%N) {assert(N>1);}
  //  Copy constructor
  inline vnl_finite_field_number(Base const& x) : val_(int(x)) {}
  //  Destructor
  inline ~vnl_finite_field_number() {}
  // Implicit conversions
  inline operator short() const { short r = (short)val_; assert(r == val_); return r; }
  inline operator short() { short r = (short)val_; assert(r == val_); return r; }
  inline operator int() const { return val_; }
  inline operator int() { return val_; }
  inline operator long() const { return val_; }
  inline operator long() { return val_; }

  //: Assignment
  inline Base& operator=(Base const& x) { val_ = int(x); return *this; }
  inline Base& operator=(int x) { val_ = x<0 ? N-1-((-x-1)%N) : x%N; return *this; }

  //: Comparison of finite field numbers.
  // Note that a finite field has no order, so < and > make no sense.
  inline bool operator==(Base const& x) const { return val_ == int(x); }
  inline bool operator!=(Base const& x) const { return val_ != int(x); }
  inline bool operator==(int x) const { return operator==(Base(x)); }
  inline bool operator!=(int x) const { return !operator==(x); }

  //: Unary minus - returns the additive inverse
  inline Base operator-() const { return Base(N-val_); }
  //: Unary plus - returns the current number
  inline Base operator+() const { return *this; }
  //: Unary not - returns true if finite field number is equal to zero.
  inline bool operator!() const { return val_ == 0; }

  //: Plus/assign: replace lhs by lhs + rhs
  inline Base& operator+=(Base const& r) { val_ += int(r); if (val_ >= N) val_ -= N; return *this; }
  inline Base& operator+=(int r) { return operator=(val_+r); }
  //: Minus/assign: replace lhs by lhs - rhs
  inline Base& operator-=(Base const& r) { val_ -= int(r); if (val_ < 0) val_ += N; return *this; }
  inline Base& operator-=(int r) { return operator=(val_-r); }
  //: Multiply/assign: replace lhs by lhs * rhs
  inline Base& operator*=(int r) { return operator=(val_*r); }
  inline Base& operator*=(Base const& r) { return operator*=(int(r)); }
  //: Divide
  inline Base operator/(Base const& r) const {
    assert(r!=0); assert(Base::is_field());
    for (int i=0; i<N; ++i) if (operator==(int(r)*i)) return i; return 0;
  }
  //: Divide
  inline Base operator/(int r) const {
    assert(r!=0); assert(Base::is_field());
    for (int i=0; i<N; ++i) if (operator==(r*i)) return i; return 0;
  }
  //: Divide/assign: replace lhs by lhs / rhs
  inline Base& operator/=(Base const& r) { assert(Base::is_field()); return operator=(*this / r); }
  inline Base& operator/=(int r) { assert(Base::is_field()); return operator=(*this / r); }

  //: Pre-increment (++r).
  inline Base& operator++() { ++val_; if (val_==N) val_=0; return *this; }
  //: Pre-decrement (--r).
  inline Base& operator--() { if (val_==0) val_=N; --val_; return *this; }
  //: Post-increment (r++).
  inline Base operator++(int){Base b=*this; ++val_; if (val_==N) val_=0; return b; }
  //: Post-decrement (r--).
  inline Base operator--(int){Base b=*this; if (val_==0) val_=N; --val_; return b;}

  //: Return true when N is a prime number, i.e., when this ring is a field
  static inline bool is_field() {
    if (N<2) return false; if (N==2 || N==3 || N==5) return true;
    if (N%2==0 || N%3==0 || N%5==0) return false;
    for (int i=3; i*i<N; i+=2) if (N%i==0) return false; return true;
  }

private:
  int val_; //!< vamue of this number (smallest nonnegative representation)
};

//: formatted output
template <int N>
inline vcl_ostream& operator<< (vcl_ostream& s, vnl_finite_field_number<N> const& r) {
  return s << int(r);
}

//: simple input
template <int N>
inline vcl_istream& operator>> (vcl_istream& s, vnl_finite_field_number<N>& r) {
  int n; s >> n; r=n; return s;
}

//: Returns the addition of two finite field numbers.
template <int N>
inline vnl_finite_field_number<N> operator+ (vnl_finite_field_number<N> const& r1, vnl_finite_field_number<N> const& r2) {
  vnl_finite_field_number<N> result(r1); return result += r2;
}
template <int N>
inline vnl_finite_field_number<N> operator+ (vnl_finite_field_number<N> const& r1, int r2) {
  vnl_finite_field_number<N> result(r1); return result += r2;
}
template <int N>
inline vnl_finite_field_number<N> operator+ (int r2, vnl_finite_field_number<N> const& r1) {
  vnl_finite_field_number<N> result(r1); return result += r2;
}

//: Returns the difference of two finite field numbers.
template <int N>
inline vnl_finite_field_number<N> operator- (vnl_finite_field_number<N> const& r1, vnl_finite_field_number<N> const& r2) {
  vnl_finite_field_number<N> result(r1); return result -= r2;
}
template <int N>
inline vnl_finite_field_number<N> operator- (vnl_finite_field_number<N> const& r1, int r2) {
  vnl_finite_field_number<N> result(r1); return result -= r2;
}
template <int N>
inline vnl_finite_field_number<N> operator- (int r2, vnl_finite_field_number<N> const& r1) {
  vnl_finite_field_number<N> result(-r1); return result += r2;
}

//: Returns the multiplication of two finite field numbers.
template <int N>
inline vnl_finite_field_number<N> operator* (vnl_finite_field_number<N> const& r1, vnl_finite_field_number<N> const& r2) {
  vnl_finite_field_number<N> result(r1); return result *= r2;
}
template <int N>
inline vnl_finite_field_number<N> operator* (vnl_finite_field_number<N> const& r1, int r2) {
  vnl_finite_field_number<N> result(r1); return result *= r2;
}
template <int N>
inline vnl_finite_field_number<N> operator* (int r2, vnl_finite_field_number<N> const& r1) {
  vnl_finite_field_number<N> result(r1); return result *= r2;
}

template <int N>
inline bool operator== (int  r1, vnl_finite_field_number<N> const& r2) { return r2==r1; }
template <int N>
inline bool operator!= (int  r1, vnl_finite_field_number<N> const& r2) { return r2!=r1; }

template <int N>
inline vnl_finite_field_number<N> vnl_math_squared_magnitude(vnl_finite_field_number<N> const& x) { return x*x; }
template <int N>
inline vnl_finite_field_number<N> vnl_math_sqr(vnl_finite_field_number<N> const& x) { return x*x; }
template <int N>
inline bool vnl_math_isnan(vnl_finite_field_number<N> const& ){return false;}
template <int N>
inline bool vnl_math_isfinite(vnl_finite_field_number<N> const& x){return true;} 

#endif // vnl_finite_field_number_h_
