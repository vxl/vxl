
//:
// \file
// \brief Infinite length integer numbers
// \author Ian Scott
// The  mbl_arb_length_int class  provides infinite length integer numbers and
// arithmetic, using a vcl_vector of unsigneds.

#include "mbl_arb_length_int.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>


bool mbl_arb_length_int::operator== (long rhs) const
{
  if (val_.size() > sizeof(unsigned long)) return false;
  if (sign_ != rhs>=0) return false;
  rhs = vnl_math_abs(rhs);
  unsigned i = sizeof(long);
  while (i-->0)
    if ( ((val_.size() <= i)?0:val_[i]) != ((rhs>>(i*8))&0xff)) return false;
  return true;
}


bool mbl_arb_length_int::operator== (unsigned long rhs) const
{
  if (val_.size() > sizeof(long)) return false;
  if (!sign_) return false;
  unsigned i = sizeof(long);
  while (i-->0)
    if ( ((val_.size() <= i)?0:val_[i]) != ((rhs>>(i*8))&0xff)) return false;
  return true;
}

mbl_arb_length_int& mbl_arb_length_int::operator*= (unsigned char rhs)
{
  if (rhs == 0)
  {
    val_.resize(0);
    sign_=true;
    return *this;
  }
  unsigned i = 0;
  unsigned char carry =0;
  while (i < val_.size() )
  {
    unsigned long v = rhs * val_[i] + carry;
    carry = v >> 8;
    val_[i++] = v & 0xff;
  }
  if (carry != 0) val_.push_back(carry);
  return *this;
}

mbl_arb_length_int& mbl_arb_length_int::operator/= (unsigned char rhs)
{
  if (rhs == 0) {vcl_cerr << "mbl_arb_length_int& divide by zero"<<vcl_endl; vcl_abort();}
  unsigned i = val_.size();
  unsigned long remainder =0;
  while (i-- >0)
  {
    remainder += val_[i];
    val_[i] = remainder / rhs;
    remainder %= rhs;
    remainder <<= 8;
  }
  if (val_.back() == 0) val_.pop_back();
  return *this;
}


bool abs_less(const mbl_arb_length_int &lhs, const mbl_arb_length_int &rhs)
{
  unsigned sl = lhs.val_.size();
  const unsigned sr = rhs.val_.size();
  if (sl < sr) return true;
  if (sl > sr) return false;
  while (sl-- > 0)
  {
    if (lhs.val_[sl] < rhs.val_[sl]) return true;
    if (lhs.val_[sl] > rhs.val_[sl]) return false;
  }
  return false; // they're equal
}

mbl_arb_length_int& mbl_arb_length_int::operator+= (const mbl_arb_length_int &rhs)
{
  if (sign_ == rhs.sign_)
  {
    unsigned carry=0, i=0;
    while (i < val_.size() || i < rhs.val_.size() || carry != 0)
    {
      if (i == val_.size()) val_.push_back(0);
      carry += val_[i] + ((i < rhs.val_.size())?rhs.val_[i]:0);
      val_[i] = carry & 0xff;
      carry >>= 8;
      ++i;
    }
  }
  else
  {
    if (abs_less (rhs, *this))
    {
      unsigned carry=0, i=0;
      while (i < val_.size() || i < rhs.val_.size() || carry != 0)
      {
        if (i == val_.size()) val_.push_back(0);
        if (val_[i] < ((i < rhs.val_.size())?rhs.val_[i]:0) + carry)
        {
          val_[i] = val_[i] + 0x100 - ((i < rhs.val_.size())?rhs.val_[i]:0) - carry;
          carry = 1;
        }
        else
        {
          val_[i] = val_[i] - ((i < rhs.val_.size())?rhs.val_[i]:0) - carry;
          carry = 0;
        }
        ++i;
      }
    }
    else
    {
      sign_ = ! sign_;

      unsigned carry=0, i=0;
      while (i < val_.size() || i < rhs.val_.size() || carry != 0)
      {
        if (i == val_.size()) val_.push_back(0);
        if (((i < rhs.val_.size())?rhs.val_[i]:0) < val_[i] + carry)
        {
          val_[i] = ((i < rhs.val_.size())?rhs.val_[i]:0) + 0x100 - val_[i] - carry;
          carry = 1;
        }
        else
        {
          val_[i] = ((i < rhs.val_.size())?rhs.val_[i]:0) - val_[i] - carry;
          carry = 0;
        }
        ++i;
      }
    }
    normalise();
  }
  return *this;
}

mbl_arb_length_int& mbl_arb_length_int::operator-= (const mbl_arb_length_int &rhs)
{
  if (sign_ != rhs.sign_)
  {
    unsigned carry=0, i=0;
    while (i < val_.size() || i < rhs.val_.size() || carry != 0)
    {
      if (i == val_.size()) val_.push_back(0);
      carry += val_[i] + ((i < rhs.val_.size())?rhs.val_[i]:0);
      val_[i] = carry & 0xff;
      carry >>= 8;
      ++i;
    }
  }
  else
  {
    if (abs_less (rhs, *this))
    {
      unsigned carry=0, i=0;
      while (i < val_.size() || i < rhs.val_.size() || carry != 0)
      {
        if (i == val_.size()) val_.push_back(0);
        if (val_[i] < ((i < rhs.val_.size())?rhs.val_[i]:0) + carry)
        {
          val_[i] = val_[i] + 0x100 - ((i < rhs.val_.size())?rhs.val_[i]:0) - carry;
          carry = 1;
        }
        else
        {
          val_[i] = val_[i] - ((i < rhs.val_.size())?rhs.val_[i]:0) - carry;
          carry = 0;
        }
        ++i;
      }
    }
    else
    {
      sign_ = ! sign_;

      unsigned carry=0, i=0;
      while (i < val_.size() || i < rhs.val_.size() || carry != 0)
      {
        if (i == val_.size()) val_.push_back(0);
        if (((i < rhs.val_.size())?rhs.val_[i]:0) < val_[i] + carry)
        {
          val_[i] = ((i < rhs.val_.size())?rhs.val_[i]:0) + 0x100 - val_[i] - carry;
          carry = 1;
        }
        else
        {
          val_[i] = ((i < rhs.val_.size())?rhs.val_[i]:0) - val_[i] - carry;
          carry = 0;
        }
        ++i;
      }
    }
    normalise();
  }
  return *this;
}



unsigned char mbl_arb_length_int::operator %  (unsigned char rhs)const
{
  if (rhs == 0) {vcl_cerr << "mbl_arb_length_int& divide by zero"<<vcl_endl; vcl_abort();}
  unsigned i = val_.size();
  unsigned long remainder =0;
  while (i-- >0)
  {
    remainder <<= 8;
    remainder += val_[i];
    remainder %= rhs;
  }
  return remainder;
}

//: Pre-increment (++r).  No-op when +-Inf.
mbl_arb_length_int& mbl_arb_length_int::operator++ ()
{
  unsigned i=0;
  if (sign_)
  {
    do
    {
      if (i == val_.size())
      {
        val_.push_back(1);
        return *this;
      }
      val_[i] ++;
    } while (val_[i++] == 0);
  }
  else
  {
    do {
      val_[i]--;
      if (i+1 == val_.size() && val_[i] == 0)
      {
        val_.pop_back();
        if (val_.empty()) sign_=true;
        return *this;
      }
    } while (val_[i++] == 0xff); 
  }
  return *this;
}

//: Pre-decrement (--r).
mbl_arb_length_int& mbl_arb_length_int::operator-- ()
{
  unsigned i=0;
  if (!sign_ || val_.size() == 0)
  {
    sign_ = false;
    do
    {
      if (i == val_.size())
      {
        val_.push_back(1);
        return *this;
      }
      val_[i] ++;
    } while (val_[i++] == 0);
  }
  else
  {
    do {
      val_[i]--;
      if (i+1 == val_.size() && val_[i] == 0)
      {
        val_.pop_back();
        return *this;
      }
    } while (val_[i++] == 0xff); 
  }
  return *this;
}



bool mbl_arb_length_int::operator< (mbl_arb_length_int const& rhs) const
{
  const int tl = val_.size() * (sign_?1:-1);
  const int tr = rhs.val_.size() * (rhs.sign_?1:-1);
  if (tl < tr) return true;
  if (tl > tr) return false;
  unsigned i = val_.size();
  while (i-- > 0)
  {
    if (val_[i] < rhs.val_[i]) return true;
    if (val_[i] > rhs.val_[i]) return false;
  }
  return false; // they're equal
}

bool mbl_arb_length_int::operator< (long rhs) const
{
  if (val_.size() > sizeof(long)) return !sign_;
  if (sign_ != (rhs >0)) return !sign_;
  unsigned i = sizeof(long);
  rhs = vnl_math_abs(rhs);
  while (i-->0)
  {
    const int k = ((val_.size() <= i)?0:val_[i]);
    const int l = (rhs>>(i*8))&0xff;
    if ( k > l) return !sign_;
    if ( k < l) return sign_;
  }
  return false;
}

bool mbl_arb_length_int::operator> (long rhs) const
{
  if (val_.size() > sizeof(long)) return sign_;
  if (sign_ != (rhs >0)) return sign_;
  unsigned i = sizeof(long);
  rhs = vnl_math_abs(rhs);
  while (i-->0)
  {
    const int k = ((val_.size() <= i)?0:val_[i]);
    const int l = (rhs>>(i*8))&0xff;
    if ( k < l) return !sign_;
    if ( k > l) return sign_;
  }
  return false;
}

bool mbl_arb_length_int::operator< (unsigned long rhs) const
{
  if (!sign_) return true;
  if (val_.size() > sizeof(unsigned long)) return false;
  unsigned i = sizeof(unsigned long);
  rhs = vnl_math_abs(rhs);
  while (i-->0)
  {
    const int k = ((val_.size() <= i)?0:val_[i]);
    const int l = (rhs>>(i*8))&0xff;
    if ( k < l) return true;
    if ( k > l) return false;
  }
  return false;
}

bool mbl_arb_length_int::operator> (unsigned long rhs) const
{
  if (!sign_) return false;
  if (val_.size() > sizeof(unsigned long)) return true;
  unsigned i = sizeof(unsigned long);
  rhs = vnl_math_abs(rhs);
  while (i-->0)
  {
    const int k = ((val_.size() <= i)?0:val_[i]);
    const int l = (rhs>>(i*8))&0xff;
    if ( k > l) return true;
    if ( k < l) return false;
  }
  return false;
}


static const short version_no =1;

void vsl_b_read(vsl_b_istream &bfs, mbl_arb_length_int &x)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs, x.val_);
      vsl_b_read(bfs, x.sign_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mbl_arb_length_int &) \n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

void vsl_b_write(vsl_b_ostream &bfs, mbl_arb_length_int &x)
{
  vsl_b_write(bfs, version_no);
  vsl_b_write(bfs, x.val_);
  vsl_b_write(bfs, x.sign_);
}

vcl_ostream& operator<< (vcl_ostream& s, mbl_arb_length_int const& r)
{
  mbl_arb_length_int x;
  if (r < 0ul) { s << "-"; x = -r; } else x =r;

  vcl_vector<unsigned char> xs;
  while (x > 0ul)
  {
    xs.push_back(x % (unsigned char)10);
    x /= (unsigned char)10;
  }
  if (xs.empty()) { s << "0"; return s; }
  while (!xs.empty()) { s << (int) xs.back(); xs.pop_back(); }
  return s;
}
