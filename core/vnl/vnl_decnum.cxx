// This is core/vnl/vnl_decnum.cxx
#include "vnl_decnum.h"
#include <vcl_cassert.h>

// constructor from (decimal) string.
vnl_decnum::vnl_decnum(vcl_string const& r)
: sign_('+'), data_("")
{
  char const* p = r.c_str();
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
  if (*p == '-') sign_ = '-', ++p;
  else if (*p == '+') ++p;
  while (*p == '0') ++p;
  while (*p >= '0' && *p <= '9') data_.push_back(*p++);
  if (data_ == "") sign_ = ' ';
}

// constructor from an unsigned long
vnl_decnum::vnl_decnum(unsigned long r)
: sign_('+'), data_("")
{
  if (r == 0) sign_ = ' ';
  else {
    while (r) {
      data_.insert(data_.begin(), '0'+(r%10));
      r/=10;
    }
  }
}

vnl_decnum::operator unsigned long() const
{
  unsigned long l = 0L;
  for (unsigned int i=0; i<data_.length(); ++i) { l *= 10; l += (data_.c_str()[i]-'0'); } // might overflow!!!
  return l; // forget the sign
}

vnl_decnum::operator long() const
{
  long l = 0L;
  for (unsigned int i=0; i<data_.length(); ++i) { l *= 10; l += (data_.c_str()[i]-'0'); } // might overflow!!!
  return sign_=='-' ? -l : l;
}

vnl_decnum::operator unsigned int() const
{
  unsigned int l = 0;
  for (unsigned int i=0; i<data_.length(); ++i) { l *= 10; l += (data_.c_str()[i]-'0'); } // might overflow!!!
  return l; // forget the sign
}

vnl_decnum::operator int() const
{
  int l = 0;
  for (unsigned int i=0; i<data_.length(); ++i) { l *= 10; l += (data_.c_str()[i]-'0'); } // might overflow!!!
  return sign_=='-' ? -l : l;
}

// This is "operator<" for strings
// The arguments should consist of digits only
// The shorter of the two arguments is implicitly zero-appended
static bool vnl_decnum_comp(vcl_string const& a, vcl_string const& b)
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum_comp with " << a << " and " << b << '\n';
#endif
  int i, na = a.length(), nb = b.length(), nc = na < nb ? na : nb;
  for (i = 0; i < nc; ++i) {
    if (a.c_str()[i] < b.c_str()[i]) return true;
    else if (a.c_str()[i] > b.c_str()[i]) return false;
  }
  for (; i < nb; ++i) { // in case b is longer than a
    if ('0' < b.c_str()[i]) return true;
  }
  return false; // longer a cannot be strictly smaller than b
}

bool vnl_decnum::operator< (vnl_decnum const& r) const
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum::operator< with " << data_ << " and " << r.data() << '\n';
#endif
  if (operator==(r)) return false;
  vcl_string rs = r.data();
  if (sign_=='-' && r.sign() == '-') return -r < operator-();
  else if (sign_=='-') return true;
  else if (r.sign() == '-') return false;
  else if (sign_==' ') return true;
  else if (r.sign() == ' ') return false;
  else if (data_.length() < rs.length()) return true;
  else if (data_.length() > rs.length()) return false;
  else // at this point, the two lengths are the same
    return vnl_decnum_comp(data_,rs);
}

// Returns the sum of the two arguments
// Both arguments should consist of digits only
static vnl_decnum vnl_decnum_plus(vcl_string const& a, vcl_string const& b)
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum_plus with " << a << " and " << b << '\n';
#endif
  vcl_string result = "";
  int na=a.length(), nb=b.length(), carry=0;
  for (--na,--nb; na>=0&&nb>=0; --na,--nb) {
    char c = a.c_str()[na] + (b.c_str()[nb] - '0') + carry;
    if (c > '9') c-=10, carry=1; else carry=0;
    result.insert(result.begin(), c);
  }
  for (; na>=0&&nb<0; --na) {
    char c = a.c_str()[na] + carry;
    if (c > '9') c-=10, carry=1; else carry=0;
    result.insert(result.begin(), c);
  }
  for (; nb>=0&&na<0; --nb) {
    char c = b.c_str()[nb] + carry;
    if (c > '9') c-=10, carry=1; else carry=0;
    result.insert(result.begin(), c);
  }
  if (carry) result.insert(result.begin(), '1');
  return vnl_decnum(result);
}

// Returns the difference of the two arguments
// Both arguments should consist of digits only
// and the first one should be numerically larger than the second one
static vnl_decnum vnl_decnum_minus(vcl_string const& a, vcl_string const& b)
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum_minus with " << a << " and " << b << '\n';
#endif
  vcl_string result = "";
  int na=a.length(), nb=b.length(), carry=0;
  assert(na>=nb);
  for (--na,--nb; na>=0&&nb>=0; --na,--nb) {
    char c = a.c_str()[na] - (b.c_str()[nb] - '0') - carry;
    if (c < '0') c+=10, carry=1; else carry=0;
    result.insert(result.begin(), c);
  }
  for (; na>=0&&nb<0; --na) {
    char c = a.c_str()[na] - carry;
    if (c < '0') c+=10, carry=1; else carry=0;
    result.insert(result.begin(), c);
  }
  for (na=0; result.c_str()[na]=='0'; ++na) ;
  if (na) result.erase(0, na);
  assert(carry==0);
  return vnl_decnum(result);
}

vnl_decnum vnl_decnum::operator+(vnl_decnum const& r) const
{
  if (sign_ == ' ') return r;
  else if (r.sign() == ' ') return *this;
  else if (operator==(-r)) return vnl_decnum(0L);
  else if (sign_ == '-' && r.sign() == '-') return - vnl_decnum_plus(data_, r.data());
  else if (sign_ == '-' && operator<(-r)) return - vnl_decnum_minus(data_, r.data());
  else if (sign_ == '-') return vnl_decnum_minus(r.data(), data_);
  else if (r.sign() == '-' && operator>(-r)) return vnl_decnum_minus(data_, r.data());
  else if (r.sign() == '-') return - vnl_decnum_minus(r.data(), data_);
  else return vnl_decnum_plus(data_, r.data());
}

// Returns the product of the two arguments
// The first argument should consist of digits only;
// the second argument should be a single digit
static vcl_string vnl_decnum_mult(vcl_string const& a, char b)
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum_mult with " << a << " and " << b << '\n';
#endif
  vcl_string result = "";
  int na=a.length(), carry=0, bb = b-'0';
  assert(bb >= 0 && bb <= 9);
  for (--na; na>=0; --na) {
    int c = (a.c_str()[na]-'0') * bb + carry;
    assert(c >= 0 && c <= 99);
    carry = c/10; c%=10;
    result.insert(result.begin(), '0'+c);
  }
  if (carry) result.insert(result.begin(), '0'+carry);
  return result;
}

vnl_decnum vnl_decnum::operator*(vnl_decnum const& r) const
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum::operator* with " << sign_ << data_ << " and " << r.sign() << r.data() << '\n';
#endif
  int sign = (sign_==' '?0:sign_=='-'?-1:1) * (r.sign()==' '?0:r.sign()=='-'?-1:1);
  vnl_decnum result(0L);
  if (sign == 0) return result;
  vcl_string zeros = "";
  int na=data_.length();
  for (--na; na>=0; --na) {
    result += vnl_decnum(vnl_decnum_mult(r.data(), data_.c_str()[na]) + zeros);
    zeros.push_back('0');
  }
  return (sign==-1) ? -result : result;
}

// Returns the largest one-significant-digit divisor of the two arguments
// The largest multiple of b not larger than a is returned in b.
// (I.e.: the product of the original b with the returned divisor.)
// The arguments should consist of digits only
// and the first one should be numerically larger than the second one
static vcl_string vnl_decnum_div(vcl_string const& a, vcl_string& b)
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum_div with " << a << " and " << b << '\n';
#endif
  int na=a.length(), nb=b.length();
  assert(na >= nb);
  if (vnl_decnum_comp(a,b)) ++nb;
  vcl_string u = "1";
  while (nb<na) { b.push_back('0'), u.push_back('0'); ++nb; }
  vcl_string c = b;
  for (; u[0]<'9'; u[0]++) {
    vnl_decnum d = vnl_decnum_plus(c,b);
    if (vnl_decnum(a) < d) { b=c; return u; }
    c=d.data();
  }
  // if we end up here, the quotient must start with 9:
  b=c; return u;
}

vnl_decnum vnl_decnum::operator/(vnl_decnum const& r) const
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum::operator/ with " << sign_ << data_ << " and " << r.sign() << r.data() << '\n';
#endif
  assert (r != 0L);
  if (r == 1L) return *this;
  if (operator==(r)) return vnl_decnum('+',"1");
  vcl_string a = data_, b = r.data();
  int na=a.length(), nb=b.length();
  vnl_decnum result(0L);
  while (na > nb || (na == nb && !vnl_decnum_comp(a,b))) {
    vcl_string c = b;
    vcl_string d = vnl_decnum_div(a, c);
#ifdef DEBUG
    vcl_cerr << "vnl_decnum_div returns " << d << '\n';
#endif
    result += vnl_decnum(d);
    vnl_decnum m = vnl_decnum(a) - vnl_decnum(c);
    a = m.data(); na=a.length();
  }
  int sign = (sign_=='-'?-1:1) * (r.sign()=='-'?-1:1);
  return sign==-1 ? -result : result;
}

vnl_decnum vnl_decnum::operator%(vnl_decnum const& r) const
{
#ifdef DEBUG
  vcl_cerr << "Entering vnl_decnum::operator% with " << sign_ << data_ << " and " << r.sign() << r.data() << '\n';
#endif
  assert (r != 0L);
  if (r == 1L) return 0L;
  if (operator==(r)) return 0L;
  vcl_string a = data_, b = r.data();
  int na=a.length(), nb=b.length();
  while (na > nb || (na == nb && !vnl_decnum_comp(a,b))) {
    vcl_string c = b;
    vcl_string d = vnl_decnum_div(a, c);
#ifdef DEBUG
    vcl_cerr << "vnl_decnum_div returns " << d << '\n';
#endif
    vnl_decnum m = vnl_decnum(a) - vnl_decnum(c);
    a = m.data(); na=a.length();
  }
  if (na==0) return vnl_decnum(0L);
  else       return vnl_decnum(sign_,a);
}

vcl_istream& operator>>(vcl_istream& s, vnl_decnum& r)
{
#ifdef DEBUG
  vcl_cerr << "Entering operator>>(istream,vnl_decnum)\n";
#endif
  vcl_string data = "";
  int c = ' ';
  while (c == ' ' || c == '\t' || c == '\r') c=s.get(); // blank skipping
  if (c == -1 || c == '\n') { r = vnl_decnum(0L); return s; } // stop parsing at EOLN or EOF
  if (c == '-') { data = "-"; c=s.get(); }
  else if (c == '+') c=s.get();
  while (c == '0') c=s.get();
  while (c >= '0' && c <= '9') { data.push_back(c); c=s.get(); }
  r = vnl_decnum(data);
  if (c > 0) s.putback(c);
  return s;
}

