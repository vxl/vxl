// This is core/vnl/vnl_decnum.h
#ifndef vnl_decnum_h_
#define vnl_decnum_h_
//:
// \file
// \brief Infinite precision integers with decimal arithmetic
//
// The vnl_decnum class implements infinite precision integers and
// arithmetic by using a dynamically allocated string of decimals.
// Implicit conversion to the system defined types short, int, long,
// float, double and long double is supported by overloaded operator
// member functions.
// Addition and subtraction operators are performed by simple decimal
// arithmetic with checks for carry flag propagation.
// The multiplication, division, and remainder operations utilize the
// naive, commonly known decimal algorithms. Beware that these are
// considerably slower than both the operations on built-in integer
// types and those implemented in the class vnl_bignum.
//
// Since the internal representation is decimal, there is no overhead
// for converting between a vnl_decnum and a decimal character string
// representation. Hence, there is a constructor from std::string, there
// are both << and >> operators, and a cast method to std::string, all
// of them essentially no-ops.
//
// \author Peter Vanroose, ABIS Leuven
// \date   August 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_string.h>

class vnl_decnum
{
  char sign_;      // Sign of vnl_decnum ('+' or '-'; for zero, the sign is ' ')
  vcl_string data_;// The decimal data (absolute value)
                   // data_ consists of decimals (0-9) only, without
                   // leading zero. Holds even for zero, represented by "".

  // private constructor: arguments should satisfy the above constraints
  vnl_decnum(char s, vcl_string const& d) : sign_(s), data_(d) {}
 public:
  vcl_string data() const { return data_; }
  char       sign() const { return sign_; }
  //: void constructor - creates a zero vnl_decnum.
  vnl_decnum() : sign_(' '), data_("") {}
  //: Creates a vnl_decnum from an unsigned long integer.
  explicit vnl_decnum(unsigned long);
  //: Creates a vnl_decnum from a long integer.
  explicit vnl_decnum(long r)
  : sign_(r<0 ? '-' : r>0 ? '+' : ' ')
  { vnl_decnum d((unsigned long)(r<0?-r:r)); data_=d.data(); }
  //: Creates a vnl_decnum from an unsigned integer.
  explicit vnl_decnum(unsigned int r)
  : sign_(r>0 ? '+' : ' ')
  { vnl_decnum d((unsigned long)r); data_=d.data(); }
  //: Creates a vnl_decnum from an integer.
  explicit vnl_decnum(int r)
  : sign_(r<0 ? '-' : r>0 ? '+' : ' ')
  { vnl_decnum d((unsigned long)(r<0?-r:r)); data_=d.data(); }
  // Copy constructor
  vnl_decnum(vnl_decnum const& r) : sign_(r.sign_), data_(r.data_) {}
  // Constructor from string
  vnl_decnum(vcl_string const&);
  vnl_decnum(char const* r) { operator=(vcl_string(r)); }

  ~vnl_decnum() {}    // Destructor

  //: Implicit type conversion to a decimal string
  operator vcl_string() const { if (sign_==' ') return "0"; vcl_string r=data_; r.insert(r.begin(), sign_); return r; }

  operator long() const;  // type conversion
  operator unsigned long() const;  // type conversion, drop the sign
  operator int() const;  // type conversion
  operator unsigned int() const;  // type conversion, drop the sign

  //: Unary plus operator
  vnl_decnum operator+() const { return *this; }
  //: Unary minus operator
  vnl_decnum operator-() const { if (sign_==' ') return *this; else return vnl_decnum(sign_=='-'?'+':'-', data_); }

  //: Assignment operator
  vnl_decnum& operator=(const vnl_decnum& r) { sign_=r.sign(); data_=r.data(); return *this; }
  vnl_decnum& operator=(vcl_string const& r) { return operator=(vnl_decnum(r)); }
  vnl_decnum& operator=(char const* r) { return operator=(vnl_decnum(vcl_string(r))); }
  vnl_decnum& operator=(unsigned long r) { return operator=(vnl_decnum(r)); }
  vnl_decnum& operator=(         long r) { return operator=(vnl_decnum(r)); }
  vnl_decnum& operator=(unsigned  int r) { return operator=(vnl_decnum(r)); }
  vnl_decnum& operator=(          int r) { return operator=(vnl_decnum(r)); }

  //: Sum
  vnl_decnum operator+(vnl_decnum const& r) const;
  vnl_decnum operator+(vcl_string const& r) const { return operator+(vnl_decnum(r)); }
  vnl_decnum operator+(char const* r) const { return operator+(vnl_decnum(vcl_string(r))); }
  vnl_decnum operator+(unsigned long r) const { return operator+(vnl_decnum(r)); }
  vnl_decnum operator+(long r) const { return operator+(vnl_decnum(r)); }
  vnl_decnum operator+(unsigned int r) const { return operator+(vnl_decnum(r)); }
  vnl_decnum operator+(int r) const { return operator+(vnl_decnum(r)); }

  //: Difference
  vnl_decnum operator-(vnl_decnum const& r) const { return operator+(-r); }
  vnl_decnum operator-(vcl_string const& r) const { return operator-(vnl_decnum(r)); }
  vnl_decnum operator-(char const* r) const { return operator-(vnl_decnum(vcl_string(r))); }
  vnl_decnum operator-(long r) const { return operator+(vnl_decnum(-r)); }
  vnl_decnum operator-(unsigned long r) const { return operator-(vnl_decnum(r)); }
  vnl_decnum operator-(int r) const { return operator+(vnl_decnum(-r)); }
  vnl_decnum operator-(unsigned int r) const { return operator-(vnl_decnum(r)); }

  //: Product
  vnl_decnum operator*(vnl_decnum const& r) const;
  vnl_decnum operator*(vcl_string const& r) const { return operator*(vnl_decnum(r)); }
  vnl_decnum operator*(char const* r) const { return operator*(vnl_decnum(vcl_string(r))); }
  vnl_decnum operator*(long r) const { return operator*(vnl_decnum(r)); }
  vnl_decnum operator*(unsigned long r) const { return operator*(vnl_decnum(r)); }
  vnl_decnum operator*(int r) const { return operator*(vnl_decnum(r)); }
  vnl_decnum operator*(unsigned int r) const { return operator*(vnl_decnum(r)); }

  //: division operator
  // \returns integral part of quotient (long division) of this with \p r
  // \p r should not be zero (assert)
  vnl_decnum operator/(vnl_decnum const& r) const;
  vnl_decnum operator/(vcl_string const& r) const { return operator/(vnl_decnum(r)); }
  vnl_decnum operator/(char const* r) const { return operator/(vnl_decnum(vcl_string(r))); }
  vnl_decnum operator/(unsigned long r) const { return operator/(vnl_decnum(r)); }
  vnl_decnum operator/(long r) const { return operator/(vnl_decnum(r)); }
  vnl_decnum operator/(unsigned int r) const { return operator/(vnl_decnum(r)); }
  vnl_decnum operator/(int r) const { return operator/(vnl_decnum(r)); }

  //: modulo operator
  // \returns remainder of long division of this with \p r
  // \p r should not be zero (assert)
  vnl_decnum operator%(vnl_decnum const& r) const;
  vnl_decnum operator%(vcl_string const& r) const { return operator%(vnl_decnum(r)); }
  vnl_decnum operator%(char const* r) const { return operator%(vnl_decnum(vcl_string(r))); }
  vnl_decnum operator%(unsigned long r) const { return operator%(vnl_decnum(r)); }
  vnl_decnum operator%(long r) const { return operator%(vnl_decnum(r)); }
  vnl_decnum operator%(unsigned int r) const { return operator%(vnl_decnum(r)); }
  vnl_decnum operator%(int r) const { return operator%(vnl_decnum(r)); }

  vnl_decnum& operator+=(vnl_decnum const& r) { return *this = operator+(r); }
  vnl_decnum& operator-=(vnl_decnum const& r) { return *this = operator+(-r); }
  vnl_decnum& operator*=(vnl_decnum const& r) { return *this = operator*(r); }
  vnl_decnum& operator/=(vnl_decnum const& r) { return *this = operator/(r); }
  vnl_decnum& operator%=(vnl_decnum const& r) { return *this = operator%(r); }

  vnl_decnum& operator+=(vcl_string const& r) { return *this = operator+(vnl_decnum(r)); }
  vnl_decnum& operator-=(vcl_string const& r) { return *this = operator-(vnl_decnum(r)); }
  vnl_decnum& operator*=(vcl_string const& r) { return *this = operator*(vnl_decnum(r)); }
  vnl_decnum& operator/=(vcl_string const& r) { return *this = operator/(vnl_decnum(r)); }
  vnl_decnum& operator%=(vcl_string const& r) { return *this = operator%(vnl_decnum(r)); }

  vnl_decnum& operator+=(char const* r) { return *this = operator+(vcl_string(r)); }
  vnl_decnum& operator-=(char const* r) { return *this = operator-(vcl_string(r)); }
  vnl_decnum& operator*=(char const* r) { return *this = operator*(vcl_string(r)); }
  vnl_decnum& operator/=(char const* r) { return *this = operator/(vcl_string(r)); }
  vnl_decnum& operator%=(char const* r) { return *this = operator%(vcl_string(r)); }

  vnl_decnum& operator+=(unsigned long r) { return *this = operator+(vnl_decnum(r)); }
  vnl_decnum& operator-=(unsigned long r) { return *this = operator-(vnl_decnum(r)); }
  vnl_decnum& operator*=(unsigned long r) { return *this = operator*(vnl_decnum(r)); }
  vnl_decnum& operator/=(unsigned long r) { return *this = operator/(vnl_decnum(r)); }
  vnl_decnum& operator%=(unsigned long r) { return *this = operator%(vnl_decnum(r)); }

  vnl_decnum& operator+=(long r) { return *this = operator+(vnl_decnum(r)); }
  vnl_decnum& operator-=(long r) { return *this = operator+(vnl_decnum(-r)); }
  vnl_decnum& operator*=(long r) { return *this = operator*(vnl_decnum(r)); }
  vnl_decnum& operator/=(long r) { return *this = operator/(vnl_decnum(r)); }
  vnl_decnum& operator%=(long r) { return *this = operator%(vnl_decnum(r)); }

  vnl_decnum& operator+=(unsigned int r) { return *this = operator+(vnl_decnum(r)); }
  vnl_decnum& operator-=(unsigned int r) { return *this = operator-(vnl_decnum(r)); }
  vnl_decnum& operator*=(unsigned int r) { return *this = operator*(vnl_decnum(r)); }
  vnl_decnum& operator/=(unsigned int r) { return *this = operator/(vnl_decnum(r)); }
  vnl_decnum& operator%=(unsigned int r) { return *this = operator%(vnl_decnum(r)); }

  vnl_decnum& operator+=(int r) { return *this = operator+(vnl_decnum(r)); }
  vnl_decnum& operator-=(int r) { return *this = operator+(vnl_decnum(-r)); }
  vnl_decnum& operator*=(int r) { return *this = operator*(vnl_decnum(r)); }
  vnl_decnum& operator/=(int r) { return *this = operator/(vnl_decnum(r)); }
  vnl_decnum& operator%=(int r) { return *this = operator%(vnl_decnum(r)); }

  //: prefix increment (++b)
  vnl_decnum& operator++() { return *this = operator+(1L); }
  //: decrement
  vnl_decnum& operator--() { return *this = operator-(1L); }
  //: postfix increment (b++)
  vnl_decnum operator++(int) { vnl_decnum b=(*this); operator++(); return b; }
  //: decrement
  vnl_decnum operator--(int) { vnl_decnum b=(*this); operator--(); return b; }

  bool operator==(vnl_decnum const& r) const { return sign_==r.sign() && data_==r.data(); } // equality
  bool operator< (vnl_decnum const&) const; // less than
  bool operator!=(vnl_decnum const& r) const { return !operator==(r); }
  bool operator> (vnl_decnum const& r) const { return r<(*this); }
  bool operator<=(vnl_decnum const& r) const { return !operator>(r); }
  bool operator>=(vnl_decnum const& r) const { return !operator<(r); }

  bool operator==(vcl_string const& r) const { vnl_decnum d(r); return sign_==d.sign() && data_==d.data(); }
  bool operator< (vcl_string const& r) const { return operator< (vnl_decnum(r)); }
  bool operator!=(vcl_string const& r) const { return operator!=(vnl_decnum(r)); }
  bool operator> (vcl_string const& r) const { return operator> (vnl_decnum(r)); }
  bool operator<=(vcl_string const& r) const { return operator<=(vnl_decnum(r)); }
  bool operator>=(vcl_string const& r) const { return operator>=(vnl_decnum(r)); }

  bool operator==(char const* r) const { return operator==(vcl_string(r)); }
  bool operator< (char const* r) const { return operator< (vcl_string(r)); }
  bool operator!=(char const* r) const { return operator!=(vcl_string(r)); }
  bool operator> (char const* r) const { return operator> (vcl_string(r)); }
  bool operator<=(char const* r) const { return operator<=(vcl_string(r)); }
  bool operator>=(char const* r) const { return operator>=(vcl_string(r)); }

  bool operator==(unsigned long r) const { vnl_decnum d(r); return sign_==d.sign() && data_==d.data(); }
  bool operator< (unsigned long r) const { return operator< (vnl_decnum(r)); }
  bool operator!=(unsigned long r) const { return operator!=(vnl_decnum(r)); }
  bool operator> (unsigned long r) const { return operator> (vnl_decnum(r)); }
  bool operator<=(unsigned long r) const { return operator<=(vnl_decnum(r)); }
  bool operator>=(unsigned long r) const { return operator>=(vnl_decnum(r)); }

  bool operator==(long r) const { vnl_decnum d(r); return sign_==d.sign() && data_==d.data(); }
  bool operator< (long r) const { return operator< (vnl_decnum(r)); }
  bool operator!=(long r) const { return operator!=(vnl_decnum(r)); }
  bool operator> (long r) const { return operator> (vnl_decnum(r)); }
  bool operator<=(long r) const { return operator<=(vnl_decnum(r)); }
  bool operator>=(long r) const { return operator>=(vnl_decnum(r)); }

  bool operator==(unsigned int r) const { vnl_decnum d(r); return sign_==d.sign() && data_==d.data(); }
  bool operator< (unsigned int r) const { return operator< (vnl_decnum(r)); }
  bool operator!=(unsigned int r) const { return operator!=(vnl_decnum(r)); }
  bool operator> (unsigned int r) const { return operator> (vnl_decnum(r)); }
  bool operator<=(unsigned int r) const { return operator<=(vnl_decnum(r)); }
  bool operator>=(unsigned int r) const { return operator>=(vnl_decnum(r)); }

  bool operator==(int r) const { vnl_decnum d(r); return sign_==d.sign() && data_==d.data(); }
  bool operator< (int r) const { return operator< (vnl_decnum(r)); }
  bool operator!=(int r) const { return operator!=(vnl_decnum(r)); }
  bool operator> (int r) const { return operator> (vnl_decnum(r)); }
  bool operator<=(int r) const { return operator<=(vnl_decnum(r)); }
  bool operator>=(int r) const { return operator>=(vnl_decnum(r)); }

  inline vnl_decnum abs() const { return sign_=='-' ? operator-() : *this; }
};

//: decimal output
// \relatesalso vnl_decnum
vcl_ostream& operator<<(vcl_ostream& s, vnl_decnum const& r)
{ return s << vcl_string(r); }

//: decimal input
// \relatesalso vnl_decnum
vcl_istream& operator>>(vcl_istream& s, vnl_decnum& r);

inline vnl_decnum vnl_math_abs(vnl_decnum const& x) { return x.abs(); }
inline vnl_decnum vnl_math_squared_magnitude(vnl_decnum const& x) { return x*x; }
inline vnl_decnum vnl_math_sqr(vnl_decnum const& x) { return x*x; }
inline bool vnl_math_isnan(vnl_decnum const&) { return false; }
inline bool vnl_math_isfinite(vnl_decnum const& x) { return true; }

#endif // vnl_decnum_h_
