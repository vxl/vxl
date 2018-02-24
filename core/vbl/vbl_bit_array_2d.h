// This is core/vbl/vbl_bit_array_2d.h
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
// Peter Vanroose - 4dec01 - adapted interface to that of vbl_array_2d<T>
// Peter Vanroose - 26 Aug 2004 - adapted interface to that of vbl_array_2d<T>
// \endverbatim
//-----------------------------------------------------------------------------

#include <iosfwd>
#include <vcl_compiler.h>

//: simple 2D bit array
// essentially identical to vbl_array_2d<bool> but more efficiently stored
class vbl_bit_array_2d
{
 public:
  // Default constructor
  vbl_bit_array_2d() : data_(VXL_NULLPTR), num_rows_(0), num_cols_(0) {}
  //: Construct num_rows x num_cols array and leave data uninitialised
  vbl_bit_array_2d(vxl::indexsize_t m, vxl::indexsize_t n) { construct(m,n); }
  //: Construct num_rows x num_cols array and fill all cells with v
  vbl_bit_array_2d(vxl::indexsize_t m, vxl::indexsize_t n, bool v) { construct(m,n); fill(v); }
  //: Construct num_rows x num_cols array and fill all cells with v
  vbl_bit_array_2d(vxl::indexsize_t m, vxl::indexsize_t n, bool v[]);
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
  void resize(vxl::indexsize_t m, vxl::indexsize_t n) { destruct(); construct(m,n); }
  //: Resizes and pads with zeros; keeps existing data
  void enlarge(vxl::indexsize_t m, vxl::indexsize_t n);
  //: make as if default-constructed.
  void clear() { if (data_) { destruct(); construct(0,0); } }

  // Data Access---------------------------------------------------------------
  bool operator() (vxl::indexsize_t i, vxl::indexsize_t j) const;
  bool operator() (vxl::indexsize_t i, vxl::indexsize_t j);

  void put(vxl::indexsize_t i, vxl::indexsize_t j, bool const &x);
  bool get(vxl::indexsize_t i, vxl::indexsize_t j) const;
  //: Set the value of a cell; default is to set the value on
  void set(vxl::indexsize_t i, vxl::indexsize_t j, bool v=true) { put(i, j, v); }
  //: Change the value of a cell
  void flip(vxl::indexsize_t i, vxl::indexsize_t j) { put(i, j, !get(i,j)); }

  inline vxl::indexsize_t rows() const { return num_rows_; }
  inline vxl::indexsize_t cols() const { return num_cols_; }
  inline vxl::indexsize_t columns() const { return num_cols_; }
  //: Number of bytes allocated by the data
  unsigned long size() const;

 private:
  unsigned char *data_;
  vxl::indexsize_t num_rows_;
  vxl::indexsize_t num_cols_;

  void destruct() { delete[] data_; data_=VXL_NULLPTR; }
  void construct(vxl::indexsize_t m, vxl::indexsize_t n);

  //helper
  void index( vxl::indexsize_t x, vxl::indexsize_t y, unsigned long &byteindex, vxl::indexsize_t &bitindex) const;
};

std::ostream& operator<< (std::ostream& os, const vbl_bit_array_2d &v);

#endif // vbl_bit_array_2d_h_
