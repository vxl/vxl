// This is core/vbl/vbl_bit_array_2d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vbl_bit_array_2d.h"

#include <vcl_iostream.h>
#include <vcl_climits.h>  // for CHAR_BIT
#include <vcl_cassert.h>
#include <vcl_cstring.h> // for memcmp()

//: Copy constructor
vbl_bit_array_2d::vbl_bit_array_2d(vbl_bit_array_2d const& that)
  : data_(0), num_rows_(0), num_cols_(0)
{
  if ( that.data_)
  {
    construct(that.num_rows_, that.num_cols_);
    vcl_memcpy(data_, that.data_, this->size());
  }
}

vbl_bit_array_2d::vbl_bit_array_2d(unsigned int m, unsigned int n, bool v[])
{
  construct(m,n);
  for (unsigned int x=0; x<m; ++x)
    for (unsigned int y=0; y<n; ++y)
      set(x,y, v[m*y+x]);
}

//: Assignment operator
vbl_bit_array_2d& vbl_bit_array_2d::operator=(vbl_bit_array_2d const& that)
{
  if (num_rows_ != that.num_rows_ ||
      num_cols_ != that.num_cols_)
    resize(that.num_rows_, that.num_cols_);

  vcl_memcpy(data_, that.data_, this->size());
  return *this;
}

//: Resizes and pads with zeros; keeps existing data
void vbl_bit_array_2d::enlarge( unsigned int num_rows, unsigned int num_cols)
{
  assert (num_rows >= num_rows_ && num_cols >= num_cols_);

  unsigned char *tempdata= data_;
  unsigned int tempm= num_rows_;
  unsigned int tempn= num_cols_;

  construct( num_rows, num_cols);
  fill(false); // fill with zeros

  if (tempdata)
  {
    for (unsigned int i=0; i< tempm; ++i)
    {
      // find start of new column
      unsigned long byteindex;
      unsigned int bitindex;
      index( i, 0, byteindex, bitindex);

      // find start of old column
      unsigned long oldbyteindex= (unsigned long)(double(i*tempn)/CHAR_BIT);

      // copy i-th column
      vcl_memcpy(data_+byteindex, tempdata+oldbyteindex, (tempn+CHAR_BIT-1)/CHAR_BIT);
    }
    delete[] tempdata;
  }
}

//: Fill with value
void vbl_bit_array_2d::fill(bool value)
{
  register unsigned char v = value ? ~(unsigned char)0 : 0;
  vcl_memset(data_, v, this->size());
}

unsigned long vbl_bit_array_2d::size() const
{
  return (num_rows_*num_cols_+CHAR_BIT-1)/CHAR_BIT;
}

void vbl_bit_array_2d::construct(unsigned int num_rows, unsigned int num_cols)
{
  // quick return if possible
  if (num_rows==0 || num_cols==0) { num_rows_=num_cols_=0; data_ = 0; return; }
  num_rows_ = num_rows;
  num_cols_ = num_cols;
  data_ = new unsigned char [this->size()];
  data_[this->size()-1]=0; // avoids uninitialized data problems in operator==()
}

void vbl_bit_array_2d::index( unsigned int x, unsigned int y, unsigned long &byteindex, unsigned int &bitindex) const
{
  unsigned long idx= x* num_cols_ + y;

  byteindex= (unsigned long)(double(idx)/CHAR_BIT);
  bitindex = idx%CHAR_BIT;
}

bool vbl_bit_array_2d::operator==(vbl_bit_array_2d const &a) const
{
  if (rows() != a.rows() || cols() != a.cols())
    return false;
  return 0 == vcl_memcmp(data_, a.data_, this->size());
}

bool vbl_bit_array_2d::operator() (unsigned int i, unsigned int j) const
{
  unsigned long byteindex;
  unsigned int bitindex;
  index( i, j, byteindex, bitindex);

  unsigned char mask= (1<<bitindex);

  return (data_[byteindex] & mask) != 0;
}

bool vbl_bit_array_2d::operator() (unsigned int i, unsigned int j)
{
  unsigned long byteindex;
  unsigned int bitindex;
  index( i, j, byteindex, bitindex);

  unsigned char mask= (1<<bitindex);

  return (data_[byteindex] & mask) != 0;
}

void vbl_bit_array_2d::put(unsigned int i, unsigned int j, bool const &x)
{
  unsigned long byteindex;
  unsigned int bitindex;

  index( i, j, byteindex, bitindex);

  unsigned char mask= (((unsigned char )(x))<<bitindex);
  unsigned char nmask= ~(((unsigned char )(1))<<bitindex);

  data_[byteindex]= mask+(nmask & data_[byteindex]);
}

bool vbl_bit_array_2d::get(unsigned int i, unsigned int j) const
{
  return operator()(i,j);
}

//
vcl_ostream& operator<< (vcl_ostream &os, const vbl_bit_array_2d &array)
{
  for (unsigned int i=0; i< array.rows(); i++)
  {
    for (unsigned int j=0; j< array.columns(); j++)
      os << array(i,j) << ' ';

    os << vcl_endl;
  }
  return os;
}
