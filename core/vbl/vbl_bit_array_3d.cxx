//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//:
// \file
// \author
// Author: Geoffrey Cross, Oxford RRG
// Created: 17 Jul 99
//
//-----------------------------------------------------------------------------

#include <vcl_cstring.h> // for memset()
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_climits.h>

#include "vbl_bit_array_3d.h"

// Default ctor
vbl_bit_array_3d::vbl_bit_array_3d(unsigned int sizex,
                                   unsigned int sizey,
                                   unsigned int sizez)
  : vbl_bit_array_3d_base(sizex, sizey, sizez),
    data_(new unsigned char[(sizex*sizey*sizez+CHAR_BIT-1)/CHAR_BIT])
{
}

vbl_bit_array_3d::vbl_bit_array_3d(unsigned int sizex,
                                   unsigned int sizey,
                                   unsigned int sizez,
                                   bool v)
  : vbl_bit_array_3d_base(sizex, sizey, sizez),
    data_(new unsigned char[(sizex*sizey*sizez+CHAR_BIT-1)/CHAR_BIT])
{
  fill(v);
}

vbl_bit_array_3d::~vbl_bit_array_3d()
{
  delete[] data_;
}

void vbl_bit_array_3d::set(unsigned int i1, unsigned int i2, unsigned int i3, bool v)
{
  unsigned long byteindex;
  unsigned char bitindex;

  index(i1, i2, i3, byteindex, bitindex);

  unsigned char mask = v ? (1<<bitindex) : 0;
  unsigned char nmask = ~(1<<bitindex);

  data_[byteindex] = mask|(nmask & data_[byteindex]);
}

void vbl_bit_array_3d::flip(unsigned int i1, unsigned int i2, unsigned int i3)
{
  unsigned long byteindex;
  unsigned char bitindex;

  index(i1, i2, i3, byteindex, bitindex);

  unsigned char mask = (data_[byteindex] & (1<<bitindex)) ? 0 : (1<<bitindex);
  unsigned char nmask = ~(1<<bitindex);

  data_[byteindex] = mask|(nmask & data_[byteindex]);
}

void vbl_bit_array_3d::fill(bool v)
{
  unsigned char temp=0;
  if (v)
    for (int t=0; t< CHAR_BIT; t++)
      temp = (temp<<1)+1;

  unsigned long memsize=(row1_count()*row2_count()*row3_count()+CHAR_BIT-1)/CHAR_BIT;
  vcl_memset(data_, temp, memsize);
}

bool vbl_bit_array_3d::operator() (unsigned int i1, unsigned int i2, unsigned int i3) const
{
  unsigned long byteindex;
  unsigned char bitindex;

  index(i1, i2, i3, byteindex, bitindex);
  unsigned char mask = (1<<bitindex);

  return (data_[byteindex] & mask) != 0;
}

void vbl_bit_array_3d_base::index(unsigned x, unsigned y, unsigned z,
                                  unsigned long & byteindex,
                                  unsigned char & bitindex) const
{
  assert(int(x)<row1_count() && int(y)<row2_count() && int(z)<row3_count());
  unsigned long i = (z*row2_count()+y)*row1_count()+x;

  byteindex = i/CHAR_BIT;
  bitindex  = i%CHAR_BIT;
}


////////////////////////////////////////////////////////////////////////

vcl_ostream &operator<<(vcl_ostream& os, vbl_bit_array_3d const& bitarray)
{
  for (int i=0; i< bitarray.row1_count(); i++)
  {
    for (int j=0; j< bitarray.row2_count(); j++)
    {
      for (int k=0; k< bitarray.row3_count(); k++)
        os << (bitarray(k,j,i) ? 'x' : '.');
      os << vcl_endl;
    }
    os << vcl_endl;
  }
  return os;
}
