// This is vxl/vbl/vbl_bit_array_2d.h
#ifndef vbl_bit_array_2d_h_
#define vbl_bit_array_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Contains class for a 2d bit array; interface as vbl_array_2d<T>
// \author Geoffrey Cross
//
// \verbatim
// Modifications
// Peter Vanroose -4dec01- adapted interface to that of vbl_array_2d<T>
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>

//: simple 2D bit array
// essentially identical to vbl_array_2d<bool> but more efficiently stored
class vbl_bit_array_2d
{
 public:
  // Default constructor
  vbl_bit_array_2d() : data_(0), num_rows_(0), num_cols_(0) {}
  //: Construct num_rows x num_cols array and leave data uninitialised
  vbl_bit_array_2d(int m, int n) { construct(m,n); }
  //: Construct num_rows x num_cols array and fill all cells with v
  vbl_bit_array_2d(int m, int n, bool v) { construct(m,n); fill(v); }
  //: Copy constructor
  vbl_bit_array_2d(vbl_bit_array_2d const &);
  // Destructor
 ~vbl_bit_array_2d() { destruct(); }

  //: Assignment operator
  vbl_bit_array_2d& operator=(vbl_bit_array_2d const&);

  //: Comparison
  bool operator==(vbl_bit_array_2d const &a) const;
  //: 
  bool operator!=(vbl_bit_array_2d const &a) const { return ! operator==(a); }

  // Operations----------------------------------------------------------------

  //: Fill with value
  void fill(bool value);
  //: Delete contents and resize to m rows x n cols
  void resize(int m, int n) { destruct(); construct(m,n); }
  //: Resizes and pads with zeros; keeps existing data
  void enlarge(int m, int n);
  //: make as if default-constructed.
  void clear() { if (data_) { destruct(); construct(0,0); } }

  // Data Access---------------------------------------------------------------
  bool operator() (int i, int j) const;
  bool operator() (int i, int j);

  void put(int i, int j, bool const &x);
  bool get(int i, int j) const;

  inline int rows() const { return num_rows_; }
  inline int cols() const { return num_cols_; }
  inline int columns() const { return num_cols_; }
  //: Number of bytes allocated by the data
  int size() const;

 private:
  unsigned char *data_;
  int num_rows_;
  int num_cols_;

  void destruct() { if (data_) delete[] data_; }
  void construct(int m, int n);

  //helper
  void index( unsigned int x, unsigned int y, unsigned long &byteindex, unsigned int &bitindex) const;
};

vcl_ostream& operator<< (vcl_ostream& os, const vbl_bit_array_2d &v);

#endif // vbl_bit_array_2d_h_
