#ifndef boct_loc_code_h_
#define boct_loc_code_h_

#include <vgl/vgl_point_3d.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

class boct_loc_code
{
public:

  // constructor, creates a code for the root node
  boct_loc_code(): x_loc_(0), y_loc_(0), z_loc_(0) {}
  boct_loc_code(const boct_loc_code& rhs);
  boct_loc_code(vgl_point_3d<double> p, short max_level);

  void set_code(short x_loc, short y_loc, short z_loc)
  { x_loc_=x_loc; y_loc_=y_loc; z_loc_=z_loc; }

  short child_index(short level);

  //: returns a code for a given child index. It is used at creating new children
  boct_loc_code child_loc_code(unsigned int index, short child_level);

  bool isequal(const boct_loc_code * test,short level);
  
  boct_loc_code * XOR(boct_loc_code * b);

  vgl_point_3d<double>  get_point(short max_level);

  short x_loc_;
  short y_loc_;
  short z_loc_;

private:
};

vcl_ostream& operator <<(vcl_ostream &s, boct_loc_code& code);
void vsl_b_write(vsl_b_ostream & os, const boct_loc_code& c);
void vsl_b_read(vsl_b_istream & is, boct_loc_code& c);

#endif