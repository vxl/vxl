// This is ./vxl/vnl/vnl_bignum.h
#ifndef vnl_bignum_h_
#define vnl_bignum_h_

//:
// \file
// \brief Infinite precision integers
//
// The vnl_bignum class implements near-infinite precision integers
// and arithmetic by using a dynamic bit vector. A
// vnl_bignum object will grow in size as necessary to hold its
// integer value.  Implicit conversion to the system defined
// types: short, int, long, float, and double
// is supported by overloaded operator member functions.
// Addition and subtraction operators are performed by
// simple bitwise addition and subtraction on
// unsigned short boundaries with checks for carry flag propagation.
// The multiplication, division, and remainder operations
// utilize the algorithms from Knuth's Volume 2 of "The
// Art of Computer Programming". However, despite the use of
// these algorithms and inline member functions, arithmetic
// operations on vnl_bignum objects are considerably slower than
// the built-in integer types that use hardware integer arithmetic
// capabilities.
//
// The vnl_bignum class supports the parsing of character string
// representations of all the literal number formats. The following
// table shows an example of a character string
// representation on the left and a brief description of the
// interpreted meaning on the right:
//
// Character String  Interpreted Meaning
// 1234              1234
// 1234l             1234
// 1234L             1234
// 1234u             1234
// 1234U             1234
// 1234ul            1234
// 1234UL            1234
// 01234             1234 in octal (leading 0)
// 0x1234            1234 in hexadecimal (leading 0x)
// 0X1234            1234 in hexadecimal (leading 0X)
// 123.4             123 (value truncated)
// 1.234e2           123 (exponent expanded/truncated)
// 1.234e-5          0 (truncated value less than 1)
//
// \author
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
// \verbatim
// Modifications
//  Peter Vanroose, 24 January 2002: ported to vnl from COOL
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for sqrt(double)

class vnl_bignum;

int magnitude_cmp(const vnl_bignum&, const vnl_bignum&);
void add(const vnl_bignum&, const vnl_bignum&, vnl_bignum&);
void subtract(const vnl_bignum&, const vnl_bignum&, vnl_bignum&);
void multiply_aux(const vnl_bignum&, unsigned short d, vnl_bignum&, unsigned short i);
//: Normalize for division
unsigned short normalize(const vnl_bignum&, const vnl_bignum&, vnl_bignum&, vnl_bignum&);
//: Divide digit
void divide_aux(const vnl_bignum&, unsigned short, vnl_bignum&, unsigned short&);
unsigned short estimate_q_hat(const vnl_bignum&, const vnl_bignum&, unsigned short);
//: Multiply quotient and subtract
unsigned short multiply_subtract(vnl_bignum&, const vnl_bignum&, unsigned short, unsigned short);
void divide(const vnl_bignum&, const vnl_bignum&, vnl_bignum&, vnl_bignum&);
vnl_bignum left_shift(const vnl_bignum& b1, int l);
vnl_bignum right_shift(const vnl_bignum& b1, int l);

//: formatted output
vcl_ostream& operator<<(vcl_ostream& s, vnl_bignum const& r);

//: simple input
vcl_istream& operator>>(vcl_istream& s, vnl_bignum& r);

//: Infinite precision integers
//
// The vnl_bignum class implements near-infinite precision integers
// and arithmetic by using a dynamic bit vector. A
// vnl_bignum object will grow in size as necessary to hold its
// integer value.  Implicit conversion to the system defined
// types: short, int, long, float, and double
// is supported by overloaded operator member functions.
// Addition and subtraction operators are performed by
// simple bitwise addition and subtraction on
// unsigned short boundaries with checks for carry flag propagation.
// The multiplication, division, and remainder operations
// utilize the algorithms from Knuth's Volume 2 of "The
// Art of Computer Programming". However, despite the use of
// these algorithms and inline member functions, arithmetic
// operations on vnl_bignum objects are considerably slower than
// the built-in integer types that use hardware integer arithmetic
// capabilities.
//
// The vnl_bignum class supports the parsing of character string
// representations of all the literal number formats. The following
// table shows an example of a character string
// representation on the left and a brief description of the
// interpreted meaning on the right:
//
// Character String  Interpreted Meaning
// 1234              1234
// 1234l             1234
// 1234L             1234
// 1234u             1234
// 1234U             1234
// 1234ul            1234
// 1234UL            1234
// 01234             1234 in octal (leading 0)
// 0x1234            1234 in hexadecimal (leading 0x)
// 0X1234            1234 in hexadecimal (leading 0X)
// 123.4             123 (value truncated)
// 1.234e2           123 (exponent expanded/truncated)
// 1.234e-5          0 (truncated value less than 1)
//
class vnl_bignum {
public:
  vnl_bignum();                        // Void constructor
  vnl_bignum(long);                    // Long constructor
  vnl_bignum(int);                     // Int constructor
  vnl_bignum(double);                  // Double constructor
  vnl_bignum(vnl_bignum const&);       // Copy constructor
  vnl_bignum(const char*);             // String constructor
  ~vnl_bignum();                       // Destructor

  operator short() const;              // Implicit type conversion
  operator int() const;                // Implicit type conversion
  operator long() const;               // Implicit type conversion
  operator float() const;              // Implicit type conversion
  operator double() const;             // Implicit type conversion

  vnl_bignum operator-() const;        // Unary minus operator

  vnl_bignum& operator=(const vnl_bignum&); // Assignment operator

  vnl_bignum operator<<(int l) const;  // Bit shift
  vnl_bignum operator>>(int l) const;  // Bit shift
  vnl_bignum operator+(vnl_bignum const& r) const;
  inline vnl_bignum& operator+=(vnl_bignum const& r) { return *this = operator+(r); }
  inline vnl_bignum& operator-=(vnl_bignum const& r) { return *this = operator+(-r); }
  vnl_bignum& operator*=(vnl_bignum const& r);
  vnl_bignum& operator/=(vnl_bignum const& r);
  vnl_bignum& operator%=(vnl_bignum const& r);
  inline vnl_bignum& operator<<=(int l) { return *this = *this << l; }
  inline vnl_bignum& operator>>=(int l) { return *this = *this >> l; }

  //: prefix increment (++b)
  vnl_bignum& operator++();
  //: decrement
  vnl_bignum& operator--();
  //: postfix increment (b++)
  inline vnl_bignum operator++(int) { vnl_bignum b=(*this); operator++(); return b; }
  //: decrement
  inline vnl_bignum operator--(int) { vnl_bignum b=(*this); operator--(); return b; }

  bool operator==(vnl_bignum const&) const; // equality
  bool operator< (vnl_bignum const&) const; // less than
  inline bool operator!=(vnl_bignum const& r) const { return !operator==(r); }
  inline bool operator> (vnl_bignum const& r) const { return r<(*this); }
  inline bool operator<=(vnl_bignum const& r) const { return !operator>(r); }
  inline bool operator>=(vnl_bignum const& r) const { return !operator<(r); }
  inline bool operator==(long r) const { return operator==(vnl_bignum(r)); }
  inline bool operator!=(long r) const { return !operator==(vnl_bignum(r)); }
  inline bool operator< (long r) const { return operator<(vnl_bignum(r)); }
  inline bool operator> (long r) const { return vnl_bignum(r) < (*this); }
  inline bool operator<=(long r) const { return !operator>(vnl_bignum(r)); }
  inline bool operator>=(long r) const { return !operator<(vnl_bignum(r)); }

  inline vnl_bignum abs() const { return operator<(0L) ? operator-() : *this; }

  void dump(vcl_ostream& = vcl_cout) const;     // Dump contents of vnl_bignum

  friend int magnitude_cmp(const vnl_bignum&, const vnl_bignum&);
  friend void add(const vnl_bignum&, const vnl_bignum&, vnl_bignum&);
  friend void subtract(const vnl_bignum&, const vnl_bignum&, vnl_bignum&);
  friend void multiply_aux(const vnl_bignum&, unsigned short, vnl_bignum&, unsigned short);
  friend unsigned short normalize(const vnl_bignum&, const vnl_bignum&, vnl_bignum&, vnl_bignum&);
  friend void divide_aux(const vnl_bignum&, unsigned short, vnl_bignum&, unsigned short&);
  friend unsigned short estimate_q_hat(const vnl_bignum&, const vnl_bignum&, unsigned short);
  friend unsigned short multiply_subtract(vnl_bignum&, const vnl_bignum&, unsigned short, unsigned short);
  friend void divide(const vnl_bignum&, const vnl_bignum&, vnl_bignum&, vnl_bignum&);
  friend vnl_bignum left_shift(const vnl_bignum& b1, int l);
  friend vnl_bignum right_shift(const vnl_bignum& b1, int l);
  friend vcl_ostream& operator<< (vcl_ostream&, const vnl_bignum&);

private:
  unsigned short count; // Number of data elements
  int sign;    // Sign of vnl_bignum (+,-,or 0)
  unsigned short* data;     // Pointer to data value

  void xtoBigNum(const char *s);       // convert hex to vnl_bignum
  int  dtoBigNum(const char *s);       // convert decimal to vnl_bignum
  void otoBigNum(const char *s);       // convert octal to vnl_bignum
  void exptoBigNum(const char *s);     // convert exponential to vnl_bignum

  void resize(short);                  // Resize vnl_bignum data
  vnl_bignum& trim();                  // Trim vnl_bignum data
};

//: Returns the addition of two bignum numbers.
inline vnl_bignum operator+(vnl_bignum const& r1, long r2) { return r1+vnl_bignum(r2); }
inline vnl_bignum operator+(vnl_bignum const& r1, int r2) { return r1+long(r2); }
inline vnl_bignum operator+(long r2, vnl_bignum const& r1) { return r1 + r2; }
inline vnl_bignum operator+(int r2, vnl_bignum const& r1) { return r1 + r2; }

//: Returns the difference of two bignum numbers.
inline vnl_bignum operator-(vnl_bignum const& r1, vnl_bignum const& r2) {
  return r1 + (-r2);
}
inline vnl_bignum operator-(vnl_bignum const& r1, long r2) {
  return r1 + (-r2);
}
inline vnl_bignum operator-(vnl_bignum const& r1, int r2) {
  return r1 + (-r2);
}
inline vnl_bignum operator-(long r2, vnl_bignum const& r1) {
  return -(r1 + (-r2));
}
inline vnl_bignum operator-(int r2, vnl_bignum const& r1) {
  return -(r1 + (-r2));
}

//: Returns the multiplication of two bignum numbers.
inline vnl_bignum operator*(vnl_bignum const& r1, vnl_bignum const& r2) {
  vnl_bignum result(r1); return result *= r2;
}
inline vnl_bignum operator*(vnl_bignum const& r1, long r2) {
  vnl_bignum result(r1); return result *= vnl_bignum(r2);
}
inline vnl_bignum operator*(vnl_bignum const& r1, int r2) {
  vnl_bignum result(r1); return result *= (long)r2;
}
inline vnl_bignum operator*(long r2, vnl_bignum const& r1) {
  vnl_bignum result(r1); return result *= r2;
}
inline vnl_bignum operator*(int r2, vnl_bignum const& r1) {
  vnl_bignum result(r1); return result *= (long)r2;
}

//: Returns the division of two bignum numbers.
inline vnl_bignum operator/(vnl_bignum const& r1, vnl_bignum const& r2) {
  vnl_bignum result(r1); return result /= r2;
}
inline vnl_bignum operator/(vnl_bignum const& r1, long r2) {
  vnl_bignum result(r1); return result /= r2;
}
inline vnl_bignum operator/(vnl_bignum const& r1, int r2) {
  vnl_bignum result(r1); return result /= (long)r2;
}
inline vnl_bignum operator/(long r1, vnl_bignum const& r2) {
  vnl_bignum result(r1); return result /= r2;
}
inline vnl_bignum operator/(int r1, vnl_bignum const& r2) {
  vnl_bignum result((long)r1); return result /= r2;
}

//: Returns the remainder of r1 divided by r2.
inline vnl_bignum operator%(vnl_bignum const& r1, vnl_bignum const& r2) {
  vnl_bignum result(r1); return result %= r2;
}
inline vnl_bignum operator%(vnl_bignum const& r1, long r2) {
  vnl_bignum result(r1); return result %= r2;
}
inline vnl_bignum operator%(vnl_bignum const& r1, int r2) {
  vnl_bignum result(r1); return result %= (long)r2;
}
inline vnl_bignum operator%(long r1, vnl_bignum const& r2) {
  vnl_bignum result(r1); return result %= r2;
}
inline vnl_bignum operator%(int r1, vnl_bignum const& r2) {
  vnl_bignum result((long)r1); return result %= r2;
}

inline bool operator==(long r1, vnl_bignum const& r2) { return r2==r1; }
inline bool operator!=(long r1, vnl_bignum const& r2) { return r2!=r1; }
inline bool operator< (long r1, vnl_bignum const& r2) { return r2> r1; }
inline bool operator> (long r1, vnl_bignum const& r2) { return r2< r1; }
inline bool operator<=(long r1, vnl_bignum const& r2) { return r2>=r1; }
inline bool operator>=(long r1, vnl_bignum const& r2) { return r2<=r1; }

#if defined(VCL_SGI_CC_720) || defined(VCL_SUNPRO_CC)
inline vnl_bignum vcl_abs(vnl_bignum const& x) { return x.abs(); }
#else
namespace std {
  inline vnl_bignum abs(vnl_bignum const& x) { return x.abs(); }
}
#endif

#include <vnl/vnl_complex_traits.h>

VCL_DEFINE_SPECIALIZATION
struct vnl_complex_traits<vnl_bignum>
{
  enum { isreal = true };
  static vnl_bignum conjugate(vnl_bignum x) { return x; }
  static vcl_complex<vnl_bignum> complexify(vnl_bignum x)
  { return vcl_complex<vnl_bignum>(x,vnl_bignum(0L)); }
};

#include <vnl/vnl_numeric_traits.h>

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<vnl_bignum> {
public:
  //: Additive identity
  static const vnl_bignum zero; // = 0L
  //: Multiplicative identity
  static const vnl_bignum one; // = 1L
  //: Return value of abs()
  typedef vnl_bignum abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef vnl_bignum double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

#include <vcl_complex.h>

inline vnl_bignum vnl_math_squared_magnitude(vcl_complex<vnl_bignum> const& z) { return vcl_norm(z); }
inline vnl_bignum vnl_math_abs(vcl_complex<vnl_bignum> const& z) { return vnl_bignum(vcl_sqrt(double(vcl_norm(z)))); }
inline vcl_complex<vnl_bignum> vnl_math_sqr(vcl_complex<vnl_bignum> const& z) { return z*z; }
inline vcl_ostream& operator<<(vcl_ostream& s, vcl_complex<vnl_bignum> const& z) {
  return s << '(' << z.real() << "," << z.imag() << ')'; }
inline vcl_istream& operator>>(vcl_istream& s, vcl_complex<vnl_bignum>& z) {
  vnl_bignum r, i; s >> r >> i; z=vcl_complex<vnl_bignum>(r,i); return s; }


VCL_DEFINE_SPECIALIZATION
struct vnl_complex_traits<vcl_complex<vnl_bignum> >
{
  enum { isreal = false };
  static vcl_complex<vnl_bignum> conjugate(vcl_complex<vnl_bignum> x)
  { return vcl_complex<vnl_bignum>(x.real(),-x.imag()); }
  static vcl_complex<vnl_bignum> complexify(vcl_complex<vnl_bignum> x) { return x; }
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<vcl_complex<vnl_bignum> > {
public:
  //: Additive identity
  static const vcl_complex<vnl_bignum> zero; // = 0L
  //: Multiplicative identity
  static const vcl_complex<vnl_bignum> one; // = 1L
  //: Return value of abs()
  typedef vnl_bignum abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef vcl_complex<vnl_bignum> double_t;
  //: Name of type which results from multiplying this type with a double
  typedef vcl_complex<double> real_t;
};

#endif // vnl_bignum_h_
