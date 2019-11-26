// This is core/vbl/vbl_bit_array_3d.cxx
//:
// \file
// \author
// Author: Geoffrey Cross, Oxford RRG
// Created: 17 Jul 99
//
//-----------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include "vbl_bit_array_3d.h"

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <climits>

void vbl_bit_array_3d::put(unsigned int i1, unsigned int i2, unsigned int i3, bool v)
{
  unsigned long byteindex;
  unsigned char bitindex;

  index(i1, i2, i3, byteindex, bitindex);

  auto mask = (unsigned char)(v?(1<<bitindex):0);
  auto nmask = (unsigned char)(~(1<<bitindex));

  data_[byteindex] = mask|(nmask & data_[byteindex]);
}

void vbl_bit_array_3d::flip(unsigned int i1, unsigned int i2, unsigned int i3)
{
  unsigned long byteindex;
  unsigned char bitindex;

  index(i1, i2, i3, byteindex, bitindex);

  auto mask = (unsigned char)((data_[byteindex] & (1<<bitindex)) ? 0 : (1<<bitindex));
  auto nmask = (unsigned char)(~(1<<bitindex));

  data_[byteindex] = mask|(nmask & data_[byteindex]);
}

void vbl_bit_array_3d::fill(bool v)
{
  unsigned char temp = v ? ~(unsigned char)0 : 0;
  std::memset(data_, temp, this->size());
}

bool vbl_bit_array_3d::operator() (unsigned int i1, unsigned int i2, unsigned int i3) const
{
  unsigned long byteindex;
  unsigned char bitindex;

  index(i1, i2, i3, byteindex, bitindex);
  auto mask = (unsigned char)(1<<bitindex);

  return (data_[byteindex] & mask) != 0;
}

void vbl_bit_array_3d::index(unsigned x, unsigned y, unsigned z,
                             unsigned long & byteindex,
                             unsigned char & bitindex) const
{
  assert(x<row1_count_ && y<row2_count_ && z<row3_count_);
  unsigned long i = (z*row2_count()+y)*row1_count()+x;

  byteindex = i/CHAR_BIT;
  bitindex  = (unsigned char)(i%CHAR_BIT);
}


////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream& os, vbl_bit_array_3d const& bitarray)
{
  for (unsigned int i=0; i< bitarray.row3_count(); ++i)
  {
    for (unsigned int j=0; j< bitarray.row2_count(); ++j)
    {
      for (unsigned int k=0; k< bitarray.row1_count(); ++k)
        os << (bitarray(k,j,i) ? 'x' : '.');
      os << std::endl;
    }
    os << std::endl;
  }
  return os;
}

void vbl_bit_array_3d::construct(unsigned int m, unsigned int n, unsigned int p)
{
  // quick return if possible
  if (m==0 || n==0 || p==0) { row1_count_=row2_count_=row3_count_=0; data_ = nullptr; return; }
  row1_count_ = m; row2_count_ = n; row3_count_ = p;
  data_ = new unsigned char [this->size()];
  data_[this->size()-1]=0; // avoids uninitialized data problems in operator==()
}

//: Copy constructor
vbl_bit_array_3d::vbl_bit_array_3d(vbl_bit_array_3d const& that)
  : row1_count_(0), row2_count_(0), row3_count_(0), data_(nullptr)
{
  if ( that.data_)
  {
    construct(that.row1_count_, that.row2_count_, that.row3_count_);
    std::memcpy(data_, that.data_, this->size());
  }
}

vbl_bit_array_3d::vbl_bit_array_3d(unsigned int m, unsigned int n, unsigned int p, bool v[])
{
  construct(m,n,p);
  for (unsigned int x=0; x<m; ++x)
    for (unsigned int y=0; y<n; ++y)
      for (unsigned int z=0; z<p; ++z)
        set(x,y,z, v[(n*z+y)*m+x]);
}

//: Assignment operator
vbl_bit_array_3d& vbl_bit_array_3d::operator=(vbl_bit_array_3d const& that)
{
  if (row1_count_ != that.row1_count() ||
      row2_count_ != that.row2_count() ||
      row3_count_ != that.row3_count())
    resize(that.row1_count_, that.row2_count_, that.row3_count_);

  std::memcpy(data_, that.data_, this->size());
  return *this;
}

bool vbl_bit_array_3d::operator==(vbl_bit_array_3d const &a) const
{
  if (row1_count_ != a.row1_count() ||
      row2_count_ != a.row2_count() ||
      row3_count_ != a.row3_count())
    return false;
  return 0 == std::memcmp(data_, a.data_, this->size());
}

unsigned long vbl_bit_array_3d::size() const
{
  return (row1_count_*row2_count_*row3_count_+CHAR_BIT-1)/CHAR_BIT;
}
