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

    unsigned long byteindex;
    unsigned int bitindex;
    index( num_rows_-1, num_cols_-1, byteindex, bitindex);

    for (unsigned int i=0; i<= byteindex; i++)
      data_[i] = that.data_[i];
  }
}

//: Assignment operator
vbl_bit_array_2d& vbl_bit_array_2d::operator=(vbl_bit_array_2d const& that)
{
  if (num_rows_ != that.num_rows_ ||
      num_cols_ != that.num_cols_)
    resize(that.num_rows_, that.num_cols_);

  unsigned long byteindex;
  unsigned int bitindex;
  index(num_rows_-1, num_cols_-1, byteindex, bitindex);

  for (unsigned int i=0;i< byteindex;i++)
    data_[i]= that.data_[i];

  return *this;
}

//: Resizes and pads with zeros; keeps existing data
void vbl_bit_array_2d::enlarge( int num_rows, int num_cols)
{
  assert(( num_rows> num_rows_) || ( num_cols> num_cols_));

  unsigned char *tempdata= data_;
  int tempm= num_rows_;
  int tempn= num_cols_;

  construct( num_rows, num_cols);
  fill( false);

  if (tempdata)
  {
    for (int i=0; i< tempm; i++)
    {
      // find start of new column
      unsigned long byteindex;
      unsigned int bitindex;
      index( i, 0, byteindex, bitindex);

      // find start of old column
      unsigned long idx= i* tempn;
      unsigned long oldbyteindex= (unsigned long)(double(idx)/CHAR_BIT);

      for (int j=0; j< tempn/CHAR_BIT; j++)
        data_[byteindex+j] = tempdata[oldbyteindex+j];
    }
    delete[] tempdata;
  }
}

//: Fill with value
void vbl_bit_array_2d::fill(bool value)
{
  unsigned long byteindex;
  unsigned int bitindex;
  index( num_rows_-1, num_cols_-1, byteindex, bitindex);

  register unsigned char v = 0;

  for (int i=0; i< CHAR_BIT; i++)
  {
    v <<= 1;
    if (value) v |= 1;
  }

  for (unsigned int i=0;i<= byteindex;i++)
    data_[i]= v;
}

int vbl_bit_array_2d::size() const { return rows()*cols()/CHAR_BIT; }

void vbl_bit_array_2d::construct(int num_rows, int num_cols)
{
  // ensure that we have a factor of CHAR_BIT rows and cols
  num_rows= (int((num_rows-1)/CHAR_BIT)+1)*CHAR_BIT;
  num_cols= (int((num_cols-1)/CHAR_BIT)+1)*CHAR_BIT;

  num_rows_ = num_rows;
  num_cols_ = num_cols;
  data_ = new unsigned char [num_rows*num_cols/CHAR_BIT + 1];
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
  return 0 == vcl_memcmp(data_, a.data_, size());
}

bool vbl_bit_array_2d::operator() (int i, int j) const
{
  unsigned long byteindex;
  unsigned int bitindex;
  index( i, j, byteindex, bitindex);

  unsigned char mask= (1<<bitindex);

  return (data_[byteindex] & mask) != 0;
}

bool vbl_bit_array_2d::operator() (int i, int j)
{
  unsigned long byteindex;
  unsigned int bitindex;
  index( i, j, byteindex, bitindex);

  unsigned char mask= (1<<bitindex);

  return (data_[byteindex] & mask) != 0;
}

void vbl_bit_array_2d::put(int i, int j, bool const &x)
{
  unsigned long byteindex;
  unsigned int bitindex;

  index( i, j, byteindex, bitindex);

  unsigned char mask= (((unsigned char )(x))<<bitindex);
  unsigned char nmask= ~(((unsigned char )(1))<<bitindex);

  data_[byteindex]= mask+(nmask & data_[byteindex]);
}

bool vbl_bit_array_2d::get(int i, int j) const
{
  return operator()(i,j);
}

//
vcl_ostream& operator<< (vcl_ostream &os, const vbl_bit_array_2d &array)
{
  for (int i=0; i< array.rows(); i++)
  {
    for (int j=0; j< array.columns(); j++)
      os << array(i,j) << ' ';

    os << vcl_endl;
  }
  return os;
}

