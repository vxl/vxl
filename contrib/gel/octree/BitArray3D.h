//-*- c++ -*-------------------------------------------------------------------
#ifndef BitArray3D_h_
#define BitArray3D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : BitArray3D
//
// .SECTION Description
//    BitArray3D is a class that Geoff hasn't documented properly. FIXME
//
// .NAME        BitArray3D - Undocumented class FIXME
// .LIBRARY     spacecarving
// .HEADER	src Package
// .INCLUDE     spacecarving/BitArray3D.h
// .FILE        BitArray3D.h
// .FILE        BitArray3D.C
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 17 Jul 99
//
//-----------------------------------------------------------------------------


#include <vcl/vcl_iosfwd.h>

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

  // Operations----------------------------------------------------------------
  
  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  void set( unsigned int i1, unsigned int i2, unsigned int i3, const bool v);
  void flip( unsigned int i1, unsigned int i2, unsigned int i3);
  bool operator() (unsigned int i1, unsigned int i2, unsigned int i3) const;

  void fill( const bool v);

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------

  
  unsigned char *data;
  unsigned long maxindex;

  // Helpers-------------------------------------------------------------------

  void index( unsigned int x, unsigned int y, unsigned int z, unsigned long &byteindex, unsigned int &bitindex) const;
};

ostream &operator<<(ostream &os, const BitArray3D &bitarray);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS BitArray3D.

