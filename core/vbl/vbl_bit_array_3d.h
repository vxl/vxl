// This is core/vbl/vbl_bit_array_3d.h
#ifndef vbl_bit_array_3d_h_
#define vbl_bit_array_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief contains classes vbl_bit_array_3d_base and vbl_bit_array_3d
// \author
//     Geoffrey Cross, Oxford RRG, 17 Jul 99
//
// \verbatim
//  Modifications
//   990717 Geoff Initial version.
//   011023 Peter Vanroose - renamed and moved to vbl
//   040826 Peter Vanroose - adapted interface to that of vbl_array_3d<T>
//\endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>

class vbl_bit_array_3d
{
 public:
  // Constructors/Destructor---------------------------------------------------

  //: Create a bitarray of the specified size, without initialising elements
  vbl_bit_array_3d(unsigned int sizex, unsigned int sizey, unsigned int sizez)
  { construct(sizex, sizey, sizez); }

  //: Create a bitarray of the specified size, with initialisation of elements
  vbl_bit_array_3d(unsigned int sizex, unsigned int sizey, unsigned int sizez, bool v)
  { construct(sizex, sizey, sizez); fill(v); }
  //: Create a bitarray of the specified size, with initialisation of elements
  vbl_bit_array_3d(unsigned int sizex, unsigned int sizey, unsigned int sizez, bool v[]);
  //: Copy constructor
  vbl_bit_array_3d(vbl_bit_array_3d const &);
  // Destructor
  ~vbl_bit_array_3d() { delete[] data_; }

  //: Assignment operator
  vbl_bit_array_3d& operator=(vbl_bit_array_3d const&);

  //: Comparison
  bool operator==(vbl_bit_array_3d const &a) const;
  //:
  bool operator!=(vbl_bit_array_3d const &a) const { return ! operator==(a); }

  // Data Access---------------------------------------------------------------

  //: Set all cell values to v
  void fill(bool v);

  //: Delete contents and resize to m rows x n cols x p layers
  void resize(unsigned int m, unsigned int n, unsigned int p) { destruct(); construct(m,n,p); }

  //: make as if default-constructed.
  void clear() { if (data_) { destruct(); row1_count_=row2_count_=row3_count_=0; } }

  //: Set the value of a cell
  void put(unsigned int i1, unsigned int i2, unsigned int i3, bool v);

  //: Set the value of a cell; default is to set the value on
  void set(unsigned int i1, unsigned int i2, unsigned int i3, bool v=true)
  { put(i1, i2, i3, v); }

  //: Return the value of a cell
  bool get(unsigned int i1, unsigned int i2, unsigned int i3) const;

  //: Change the value of a cell
  void flip(unsigned int i1, unsigned int i2, unsigned int i3);

  //: Return the value of a cell
  bool operator() (unsigned int i1, unsigned int i2, unsigned int i3) const;

  unsigned int row1_count() const { return row1_count_; }
  unsigned int row2_count() const { return row2_count_; }
  unsigned int row3_count() const { return row3_count_; }
  //: Number of bytes allocated by the data
  unsigned long size() const;

 private:
  // Data Members--------------------------------------------------------------

  unsigned int row1_count_;
  unsigned int row2_count_;
  unsigned int row3_count_;
  unsigned char *data_;

  // Helpers-------------------------------------------------------------------

  void destruct() { delete[] data_; data_=0; }
  void construct(unsigned int m, unsigned int n, unsigned int p);

  void index(unsigned int x, unsigned int y, unsigned int z,
             unsigned long &byteindex, unsigned char &bitindex) const;
};

vcl_ostream &operator<<(vcl_ostream &os, vbl_bit_array_3d const&);

#endif // vbl_bit_array_3d_h_
