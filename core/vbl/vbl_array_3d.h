#ifndef vbl_array_3dh
#define vbl_array_3dh
#ifdef __GNUG__
#pragma interface
#endif

// .LIBRARY vbl
// .HEADER Basics package
// .INCLUDE vbl/vbl_array_3d.h
// .FILE vbl/vbl_array_3d.cxx
//
// .SECTION Author:
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//
// .SECTION Modifications:
//             960926 AWF Converted to non-fascist C++ :-)
//             970218 AWF Templated
//-------------------------------------------------------------------------------

#include <vbl/vbl_base_array_3d.h>

#ifdef __OPTIMIZE__
#define RANGECHECK(i,j,k) ((void)0)
#else
#include <vcl/vcl_cassert.h>
#define RANGECHECK(i,j,k) assert(((i1 < _row1_count) && (i2 < _row2_count) && (i3 < _row3_count)) || !"vbl_array_3d Rangecheck")
#endif

//: Templated 3-dimensional array
// vbl_array_3d holds a 3-dimensional array of doubles, and provides access via
// get(i,j,k), and set(i,j,k, value) methods.
//
template <class T>
class vbl_array_3d : public vbl_base_array_3d {
  // PUBLIC INTERFACE----------------------------------------------------------
public:
  typedef vbl_base_array_3d base;

  // Constructors/Initializers/Destructors-------------------------------------
  
  vbl_array_3d(unsigned int n1, unsigned int n2, unsigned int n3);
  vbl_array_3d(unsigned int n1, unsigned int n2, unsigned int n3, const T* init_values);
  vbl_array_3d(unsigned int n1, unsigned int n2, unsigned int n3, const T& fillvalue);
 ~vbl_array_3d ();

  vbl_array_3d(const vbl_array_3d& that);
  vbl_array_3d& operator = (const vbl_array_3d& that);
  
  // Data Access---------------------------------------------------------------

        T& operator() (unsigned i1, unsigned i2, unsigned i3)       { RANGECHECK(i1,i2,i3); return _element [i1][i2][i3]; }
  const T& operator() (unsigned i1, unsigned i2, unsigned i3) const { RANGECHECK(i1,i2,i3); return _element [i1][i2][i3]; }

  // data_block will return all elements of the array in sequential storage.
        T* data_block()       { return _element[0][0]; }
  const T* data_block() const { return _element[0][0]; }

  // Data Control--------------------------------------------------------------
  
  // Utility Methods-----------------------------------------------------------
  void set(const T* static_array);
  void get(T* static_array) const;
  void fill(const T& value);
  
  // INTERNALS-----------------------------------------------------------------

protected:
  void allocate_array (unsigned int row1_count, unsigned int row2_count, unsigned int row3_count);
  // Data Members--------------------------------------------------------------

private:

  T ***_element;
};

#undef RANGECHECK

//
// formatted I/O
//
#include <vcl/vcl_iosfwd.h>
template <class T> ostream & operator<<(ostream &,const vbl_array_3d<T> &);
template <class T> istream & operator>>(istream &,vbl_array_3d<T> &);

#define VBL_ARRAY_3D_INSTANTIATE \
extern "please include vbl/vbl_array_3d.txx instead"
#define VBL_ARRAY_3D_IO_INSTANTIATE \
extern "please include vbl/vbl_array_3d.txx instead"

#endif
