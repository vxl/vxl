// This is vxl/vbl/vbl_bit_array_3d.h
#ifndef vbl_bit_array_3d_h_
#define vbl_bit_array_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 17 Jul 99
// \verbatim
// Modifications:
//   990717 Geoff Initial version.
//   011023 Peter Vanroose - renamed and moved to vbl
//\endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>

class vbl_bit_array_3d_base
{
 public:
  vbl_bit_array_3d_base(int a, int b, int c)
    : row1_count_(a),
      row2_count_(b),
      row3_count_(c)
    {}

  int row1_count() const { return row1_count_; }
  int row2_count() const { return row2_count_; }
  int row3_count() const { return row3_count_; }

 protected:
  int row1_count_;
  int row2_count_;
  int row3_count_;

  // Helpers-------------------------------------------------------------------

  void index(unsigned int x, unsigned int y, unsigned int z,
             unsigned long &byteindex, unsigned char &bitindex) const;
};


class vbl_bit_array_3d : public vbl_bit_array_3d_base
{
 public:
  // Constructors/Destructor---------------------------------------------------

  //: Create a bitarray of the specified size, without initialising elements
  vbl_bit_array_3d(unsigned int sizex, unsigned int sizey, unsigned int sizez);
  //: Create a bitarray of the specified size, with initialisation of elements
  vbl_bit_array_3d(unsigned int sizex, unsigned int sizey, unsigned int sizez, bool v);
  ~vbl_bit_array_3d();

  // Data Access---------------------------------------------------------------

  //: Set all cell values to v
  void fill(bool v);

  //: Set the value of a cell; default is to set the value on
  void set(unsigned int i1, unsigned int i2, unsigned int i3, bool v=true);

  //: Change the value of a cell
  void flip(unsigned int i1, unsigned int i2, unsigned int i3);

  //: Return the value of a cell
  bool operator() (unsigned int i1, unsigned int i2, unsigned int i3) const;

 protected:
  // Data Members--------------------------------------------------------------

  unsigned char *data_;
};

vcl_ostream &operator<<(vcl_ostream &os, vbl_bit_array_3d const&);

#endif // vbl_bit_array_3d_h_
