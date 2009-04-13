#include "boct_loc_code.h"
//:
// \file

boct_loc_code::boct_loc_code(const boct_loc_code& rhs)
{
  x_loc_ = rhs.x_loc_;
  y_loc_ = rhs.y_loc_;
  z_loc_ = rhs.z_loc_;
}

boct_loc_code::boct_loc_code(vgl_point_3d<double> p, short max_level)
{
  // root level r = n -1
  int max_val = 1 << (max_level-1);
  x_loc_ = (short) (p.x()*max_val);
  y_loc_ = (short) (p.y()*max_val);
  z_loc_ = (short) (p.z()*max_val);
}

boct_loc_code boct_loc_code::child_loc_code(unsigned int index, short child_level)
{
  short bit_idx = 1;
  short xloc = (index & bit_idx);
  xloc = (xloc << child_level) + x_loc_;

  bit_idx = 2;
  short yloc = (index & bit_idx) >> 1;
  yloc = (yloc << child_level) + y_loc_;

  bit_idx = 4;
  short zloc = (index & bit_idx) >> 2;
  zloc = (zloc << child_level) + z_loc_;
  boct_loc_code code;
  code.set_code(xloc, yloc, zloc);
  return code;
}

short boct_loc_code::child_index(short level)
{
  // level 0 cannot have a child, that ought to be the last level
  if (level == 0)
    return -1;

  // the bits are stored as [00...00ZYX]
  short child_bit = 1 << (level-1);
  short index_x = ((x_loc_ & child_bit) >> (level-1));
  short index_y = ((y_loc_ & child_bit) >> (level-2));
  short index_z = ((z_loc_ & child_bit) >> (level-3));
  return index_x+index_y+index_z;
}

vcl_ostream& operator <<(vcl_ostream &s, boct_loc_code& code)
{
  s << '[' << code.x_loc_ << ',' << code.y_loc_ << ',' << code.z_loc_ << "] ";
  return s;
}

bool boct_loc_code::isequal(const boct_loc_code * test,short level)
{
  short relevantbit = 1 << (level-1);
  return (x_loc_ & relevantbit) == (test->x_loc_ & relevantbit) &&
         (y_loc_ & relevantbit) == (test->y_loc_ & relevantbit) &&
         (z_loc_ & relevantbit) == (test->z_loc_ & relevantbit);
}

boct_loc_code * boct_loc_code::XOR(boct_loc_code * b)
{
    boct_loc_code * xorcode=new boct_loc_code();
    xorcode->x_loc_=this->x_loc_^b->x_loc_;
    xorcode->y_loc_=this->y_loc_^b->y_loc_;
    xorcode->z_loc_=this->z_loc_^b->z_loc_;

    return xorcode;
}

//: function to convert location code to a point.
vgl_point_3d<double> boct_loc_code::get_point(short max_level)
{
    int max_val = 1 << (max_level-1);

    vgl_point_3d<double> p((double)x_loc_/(double)(max_val),
                           (double)y_loc_/(double)(max_val),
                           (double)z_loc_/(double)(max_val));

    return p;
}

void vsl_b_write(vsl_b_ostream & os, const boct_loc_code& c)
{
  const short io_version_no = 1;

  vsl_b_write(os, io_version_no);
  vsl_b_write(os, c.x_loc_);
  vsl_b_write(os, c.y_loc_);
  vsl_b_write(os, c.z_loc_);
}

void vsl_b_read(vsl_b_istream & is, boct_loc_code& c)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    short x, y, z;
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    vsl_b_read(is, z);
    c.set_code(x,y,z);
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boct_loc_code&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
