//-*- c++ -*-------------------------------------------------------------------
#ifndef BitArray3D_h_
#define BitArray3D_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 17 Jul 99
//
//-----------------------------------------------------------------------------


#include <vcl_iosfwd.h>

class vbl_base_array_3d
{
public:
  vbl_base_array_3d( int a, int b, int c)
    : row1_count_(a),
      row2_count_(b),
      row3_count_(c)
    {}

  int get_row1_count () const { return row1_count_; }
  int get_row2_count () const { return row2_count_; }
  int get_row3_count () const { return row3_count_; }

protected:
  int row1_count_;
  int row2_count_;
  int row3_count_;
};


class BitArray3D : public vbl_base_array_3d
{
public:
  // Constructors/Destructors--------------------------------------------------

  BitArray3D( unsigned int sizex, unsigned int sizey, unsigned int sizez);
  BitArray3D( unsigned int sizex, unsigned int sizey, unsigned int sizez, const bool v);
  ~BitArray3D();

  // Data Access---------------------------------------------------------------

  void set( unsigned int i1, unsigned int i2, unsigned int i3, const bool v);
  void flip( unsigned int i1, unsigned int i2, unsigned int i3);
  bool operator() (unsigned int i1, unsigned int i2, unsigned int i3) const;

  void fill( const bool v);

protected:
  // Data Members--------------------------------------------------------------

  unsigned char *data;
  unsigned long maxindex;

  // Helpers-------------------------------------------------------------------

  void index( unsigned int x, unsigned int y, unsigned int z, unsigned long &byteindex, unsigned int &bitindex) const;
};

vcl_ostream &operator<<(vcl_ostream &os, const BitArray3D &bitarray);

#endif // BitArray3D_h_
