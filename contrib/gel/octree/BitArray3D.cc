//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "BitArray3D.h"
#endif
//
// Class: BitArray3D
// Author: Geoffrey Cross, Oxford RRG
// Created: 17 Jul 99
// Modifications:
//   990717 Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_cstring.h>
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_climits.h>

#include "BitArray3D.h"

//#define BITS_PER_BYTE 8


// Default ctor
BitArray3D::BitArray3D( unsigned int sizex, unsigned int sizey, unsigned int sizez)
  : vbl_base_array_3d( sizex, sizey, sizez),
    data( new unsigned char[(unsigned long)(double(sizex*sizey*sizez)/CHAR_BIT)+1]),
    maxindex( row1_count_* row2_count_* row3_count_)
{
}

BitArray3D::BitArray3D( unsigned int sizex, unsigned int sizey, unsigned int sizez, const bool v)
  : vbl_base_array_3d( sizex, sizey, sizez),
    data( new unsigned char[(unsigned long)(double(sizex*sizey*sizez)/CHAR_BIT)+1]),
    maxindex( row1_count_* row2_count_* row3_count_)
{
  fill( v);
}

BitArray3D::~BitArray3D()
{
  delete[] data;
}

void BitArray3D::set( unsigned int i1, unsigned int i2, unsigned int i3, const bool v)
{
  unsigned long byteindex;
  unsigned int bitindex;

  index( i1, i2, i3, byteindex, bitindex);

  unsigned char mask= (((unsigned char )(v))<<bitindex);
  unsigned char nmask= ~(((unsigned char )(1))<<bitindex);
  
  data[byteindex]= mask+(nmask & data[byteindex]);
}

void BitArray3D::flip( unsigned int i1, unsigned int i2, unsigned int i3)
{
  unsigned long byteindex;
  unsigned int bitindex;

  index( i1, i2, i3, byteindex, bitindex);
  unsigned char mask= (1<<bitindex);

  bool newvalue= !bool((data[byteindex] & mask));
  unsigned char nmask= (((unsigned char )(newvalue))<<bitindex);
  unsigned char nnmask= ~(((unsigned char )(1))<<bitindex);

  data[byteindex]= nmask+(nnmask & data[byteindex]);
}

void BitArray3D::fill( const bool v)
{
  unsigned char temp= 0;

  for( int t=0; t< CHAR_BIT; t++)
    temp= (temp<<1)+v;

  unsigned long memsize( ( unsigned long)(double( row1_count_* row2_count_* row3_count_)/CHAR_BIT+1));
  memset( data, temp, memsize);
}

bool BitArray3D::operator() (unsigned int i1, unsigned int i2, unsigned int i3) const
{
  unsigned long byteindex;
  unsigned int bitindex;

  index( i1, i2, i3, byteindex, bitindex);
  unsigned char mask= (1<<bitindex);

  return bool(data[byteindex] & mask);
}

void BitArray3D::index( unsigned x, unsigned y, unsigned z, unsigned long &byteindex, unsigned int &bitindex) const
{
  unsigned long index= z* row1_count_* row2_count_+y* row1_count_+x;
  assert( index< maxindex);

  byteindex= (unsigned long)(double(index)/CHAR_BIT);
  bitindex = index%CHAR_BIT;
}


////////////////////////////////////////////////////////////////////////

ostream &operator<<(ostream &os, const BitArray3D &bitarray)
{
  for( int i=0; i< bitarray.get_row1_count(); i++)
    {
      for( int j=0; j< bitarray.get_row2_count(); j++)
	{
	  for( int k=0; k< bitarray.get_row3_count(); k++)
	    {
	      os << bitarray(k,j,i) << ' ';
	    }
	  
	  os << endl;
	}
      os << endl;
    }

  return os;
}
