// This is ./vxl/vnl/mbl_arb_length_int.h
#ifndef mbl_arb_length_int_h_
#define mbl_arb_length_int_h_

//:
// \file
// \brief Infinite length integer numbers
// \author Ian Scott
// The  mbl_arb_length_int class  provides infinite length integer numbers and
// arithmetic, using a vcl_vector of unsigneds.

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vcl_vector.h>
#include <vnl/vnl_math.h>
#include <vsl/vsl_binary_io.h>

static const long mbl_arb_length_int_max_long = (unsigned char) (0xff);


//: Arbitrary length integer numbers.
// Provides infinite length integer numbers and
// arithmetic, using a vcl_vector of unsigned chars.
class mbl_arb_length_int {
public:
  //: Creates an arbitrary length integer from a vector of longs.
  // The value is sign * Sum K^i * val[i], where K = length of chars.
  mbl_arb_length_int ( bool sign, const vcl_vector<unsigned char> &val)
    : val_(val), sign_(sign) {}
  //: Default constructor gives 0.
  explicit mbl_arb_length_int (long v= 0)
    : val_(sizeof(long)), sign_(v>0)
  {
    v = vnl_math_abs(v);
    unsigned i = sizeof(long);
    while (i-->0)
      val_[i] = v>>(i*8);
    normalise();
  }
  //: Default constructor gives 0.
  explicit mbl_arb_length_int (unsigned long v)
    : val_(sizeof(long)), sign_(false)
  {
    unsigned i = sizeof(unsigned long);
    while (i-->0)
      val_[i] = v>>(i*8);
    normalise();
  }

  //  Copy constructor
  mbl_arb_length_int (mbl_arb_length_int const& from)
    : val_(from.val_), sign_(from.sign_) {};

  //  Destructor
  ~mbl_arb_length_int() {}

  //: Copies the contents and state of rhs over to the lhs
  mbl_arb_length_int& operator= (mbl_arb_length_int const& rhs) {
    val_ = rhs.val_; sign_ = rhs.sign_; return *this; }

  mbl_arb_length_int& operator*= (unsigned char rhs);

  mbl_arb_length_int& operator/= (unsigned char rhs);

  unsigned char operator %  (unsigned char rhs)const;

  //: Returns true if the two integers have the same representation
  bool operator== (mbl_arb_length_int const& rhs) const {
    return sign_ == rhs.sign_ && val_ == rhs.val_;}
  bool operator!= (mbl_arb_length_int const& rhs) const { return !operator==(rhs); }
  bool operator== (long rhs) const;
  bool operator== (unsigned long rhs) const;
  bool operator!= (long rhs) const { return !operator==(rhs); }
  bool operator!= (unsigned long rhs) const { return !operator==(rhs); }

  //: Unary minus - returns the negation of the current rational.
  mbl_arb_length_int operator-() const { return mbl_arb_length_int(!sign_, val_);}
  //: Unary plus - returns the current rational.
  mbl_arb_length_int operator+() const { return *this; }
  //: Unary not - returns true if rational is equal to zero.
  bool operator!() const { return val_.size() == 0;}
  //: Returns the absolute value of the current rational.
  mbl_arb_length_int abs() const { if (!sign_) return -(*this); else return *this; }


  //: Pre-increment (++r).  No-op when +-Inf.
  mbl_arb_length_int& operator++ ();
  //: Pre-decrement (--r).
  mbl_arb_length_int& operator-- ();
  //: Post-increment (r++).
  mbl_arb_length_int operator++(int)
  { mbl_arb_length_int b=*this; operator++ (); return b;}
  //: Post-decrement (r--).
  mbl_arb_length_int operator--(int)
  { mbl_arb_length_int b=*this; operator-- (); return b;}

  bool operator< (mbl_arb_length_int const& rhs) const;
  bool operator< (long rhs) const;
  bool operator> (long rhs) const ;
  bool operator< (unsigned long rhs) const;
  bool operator> (unsigned long rhs) const;

  bool operator> (mbl_arb_length_int const& r) const { return r < *this; }
  bool operator<= (mbl_arb_length_int const& r) const { return !operator>(r); }
  bool operator>= (mbl_arb_length_int const& r) const { return !operator<(r); }

  bool operator<= (long r) const { return !operator>(r); }
  bool operator>= (long r) const { return !operator<(r); }
  bool operator<= (unsigned long r) const { return !operator>(r); }
  bool operator>= (unsigned long r) const { return !operator<(r); }

  bool fits_in_long() const { return (val_.size()==4 && val_.back()&0x80==0) || val_.size() < 3;}
  bool fits_in_unsigned_long() const {return sign_ && val_.size() <= sizeof(unsigned long);}

  operator long () {
    assert(fits_in_long());
    long n=0;
    for (unsigned i=0;i<val_.size();i++)
      n |= val_[i] << (8*i);
    if (sign_)
      return n;
    else
      return -n;
  }
  operator unsigned long () {
    assert(fits_in_unsigned_long());
    long n=0;
    for (unsigned i=0;i<val_.size();i++)
      n |= val_[i] << (8*i);
      return n;
  }

  // Implicit conversions
  operator short () {
    long t = operator long (); short r = (short)t;
    assert(r == t); // abort on underflow or overflow
    return r;
  }
  operator int () {
    long t = operator long (); int r = (int)t;
    assert(r == t); // abort on underflow or overflow
    return r;
  }

private:
  //: The value is sign_ * Sum K^i * val_[i], where K = MAX_CHAR.
  vcl_vector<unsigned char> val_;
  //: True if positive.
  bool sign_;
  void normalise(){
    while (!val_.empty() && val_.back() ==0) val_.pop_back();
    if (val_.empty()) sign_ = true;
  }
  friend void vsl_b_read(vsl_b_istream &, mbl_arb_length_int &);
  friend void vsl_b_write(vsl_b_ostream &bfs, mbl_arb_length_int &x);
};

//: formatted output
vcl_ostream& operator<< (vcl_ostream& s, mbl_arb_length_int const& r);

inline bool operator== (int  r1, mbl_arb_length_int const& r2) { return r2==(long)r1; }
inline bool operator== (long r1, mbl_arb_length_int const& r2) { return r2==r1; }
inline bool operator!= (int  r1, mbl_arb_length_int const& r2) { return r2!=(long)r1; }
inline bool operator!= (long r1, mbl_arb_length_int const& r2) { return r2!=r1; }
inline bool operator<  (int  r1, mbl_arb_length_int const& r2) { return r2> (long)r1; }
inline bool operator<  (long r1, mbl_arb_length_int const& r2) { return r2> r1; }
inline bool operator>  (int  r1, mbl_arb_length_int const& r2) { return r2< (long)r1; }
inline bool operator>  (long r1, mbl_arb_length_int const& r2) { return r2< r1; }
inline bool operator<= (int  r1, mbl_arb_length_int const& r2) { return r2>=(long)r1; }
inline bool operator<= (long r1, mbl_arb_length_int const& r2) { return r2>=r1; }
inline bool operator>= (int  r1, mbl_arb_length_int const& r2) { return r2<=(long)r1; }
inline bool operator>= (long r1, mbl_arb_length_int const& r2) { return r2<=r1; }


#if defined(VCL_SUNPRO_CC)
inline mbl_arb_length_int vcl_abs (mbl_arb_length_int const& x) { return x.abs(); }
#else
namespace std {
  inline mbl_arb_length_int abs (mbl_arb_length_int const& x) { return x.abs(); }
}
#endif

inline mbl_arb_length_int vnl_math_abs(mbl_arb_length_int const& x) { return x<0L ? -x : x; }
inline bool vnl_math_isnan(mbl_arb_length_int const& x){return false;}
inline bool vnl_math_isfinite(mbl_arb_length_int const& x){return true;}

void vsl_b_read(vsl_b_istream &, mbl_arb_length_int &);
void vsl_b_write(vsl_b_istream &, mbl_arb_length_int &);

#endif // mbl_arb_length_int_h_
