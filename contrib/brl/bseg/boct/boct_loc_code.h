#ifndef boct_loc_code_h_
#define boct_loc_code_h_
//:
// \file
// \brief  locational code for octree.
// The code is stored separately from x, y and z dimensions.
// In that sense, it is similar or biTree in each dimension.
//
// \author Gamze Tunali
// \date April 01, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

template <typename T>
class boct_loc_code
{
 public:

  // constructor, creates a code for the root node
  boct_loc_code(): x_loc_(0), y_loc_(0), z_loc_(0),level(0) {}

  //: copy constructor
  boct_loc_code(const boct_loc_code& rhs);

  //: Constructor from a point, the root level of a tree (num_levels -1)
  boct_loc_code(vgl_point_3d<double> p, short root_level);

  //: Constructor from a point, the root level of a tree (num_levels -1) and a max_val( 2^(root_level)
  boct_loc_code(vgl_point_3d<double> p, short root_level, double max_val);

  //: set code from the dimensional data
  void set_code(T x_loc, T y_loc, T z_loc) { x_loc_=x_loc; y_loc_=y_loc; z_loc_=z_loc; }

  //: sets the level where this loc code belongs to
  void set_level(short levelnum){level=levelnum;}

  //: the bits are stored as [00...00ZYX]. level is the previous level to the child
  short child_index(short level);

  //: returns a code for a given child index. It is used at creating new children
  boct_loc_code child_loc_code(unsigned int index, short child_level);

  //: returns true if the locational codes of test is equal to this
  bool isequal(const boct_loc_code * test);

  //: returns true if the locational codes of test is equal to this
  bool isequal(const boct_loc_code & test);

  //: returns the XOR of the locational codes of this and b
  boct_loc_code * XOR(boct_loc_code * b);

  //: returns the AND of the locational codes of this and b
  boct_loc_code * AND(boct_loc_code *b);

  //: converts location code to a point.
  vgl_point_3d<double>  get_point(short max_level);

  //: locational code in X dimension
  T x_loc_;

  //: locational code in Y dimension
  T y_loc_;

  //: locational code in Z dimension
  T z_loc_;

  //: the level of the tree where this loc code belongs to
  short level;

  inline bool operator==(boct_loc_code<T>const& code) const
  {
    return code.x_loc_ == x_loc_
        && code.y_loc_ == y_loc_
        && code.z_loc_ == z_loc_
        && code.level  == level;
  }
};

template<class T>
std::ostream& operator <<(std::ostream &s, boct_loc_code<T>& code);

template<class T>
void vsl_b_write(vsl_b_ostream & os, const boct_loc_code<T>& c);

template<class T>
void vsl_b_read(vsl_b_istream & is, boct_loc_code<T>& c);

#endif
