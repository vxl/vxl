#ifndef boct_loc_code_hxx_
#define boct_loc_code_hxx_
//:
// \file
#include "boct_loc_code.h"

template <class T>
boct_loc_code<T>::boct_loc_code(const boct_loc_code<T>& rhs)
{
  x_loc_ = rhs.x_loc_;
  y_loc_ = rhs.y_loc_;
  z_loc_ = rhs.z_loc_;
  level  = rhs.level;
}

template <class T>
boct_loc_code<T>::boct_loc_code(vgl_point_3d<double> p, short root_level)
{
  double max_val = 1 << root_level;
  x_loc_ = (T) (p.x()* max_val);
  y_loc_ = (T) (p.y()* max_val);
  z_loc_ = (T) (p.z()* max_val);
  level=0;
}

template <class T>
boct_loc_code<T>::boct_loc_code(vgl_point_3d<double> p, short root_level, double max_val)
{
  if (max_val <= 0) max_val = 1 << root_level;
  x_loc_ = (T) (p.x()* max_val);
  y_loc_ = (T) (p.y()* max_val);
  z_loc_ = (T) (p.z()* max_val);
  level=0;
}

template <class T>
boct_loc_code<T> boct_loc_code<T>::child_loc_code(unsigned int index, short child_level)
{
  T bit_idx = 1;
  T xloc = (index & bit_idx);
  xloc = (xloc << child_level) + x_loc_;

  bit_idx = 2;
  T yloc = (index & bit_idx) >> 1;
  yloc = (yloc << child_level) + y_loc_;

  bit_idx = 4;
  T zloc = (index & bit_idx) >> 2;
  zloc = (zloc << child_level) + z_loc_;
  boct_loc_code<T> code;
  code.set_code(xloc, yloc, zloc);
  return code;
}


template <class T>
short boct_loc_code<T>::child_index(short this_level)
{
  // level 0 cannot have a child, that ought to be the last level
  if (this_level == 0)
    return -1;

  // the bits are stored as [00...00ZYX]
  T child_bit = 1 << (this_level-1);
  short index =0;
  if ((x_loc_ & child_bit))
    index+=1;
  if ((y_loc_ & child_bit))
    index+=2;
  if ((z_loc_ & child_bit))
    index+=4;

  return index;
}

template <class T>
std::ostream& operator <<(std::ostream &s, boct_loc_code<T>& code)
{
  s << '[' << code.x_loc_ << ',' << code.y_loc_ << ',' << code.z_loc_ << "] ";
  return s;
}

template <class T>
bool boct_loc_code<T>::isequal(const boct_loc_code<T> * test)
{
  if (test->level != level)
    return false;
#if 0  // this was not working right..
  T relevantbit = 1 << (test->level-1);
  if ( (x_loc_ & relevantbit) == (test->x_loc_ & relevantbit)&&
       (y_loc_ & relevantbit) == (test->y_loc_ & relevantbit)&&
       (z_loc_ & relevantbit) == (test->z_loc_ & relevantbit) )
#endif
  if ((x_loc_==test->x_loc_) && (y_loc_==test->y_loc_) && (z_loc_==test->z_loc_))
    return true;
  else
    return false;
}

template <class T>
bool boct_loc_code<T>::isequal(const boct_loc_code<T> &test)
{
  if (test.level != level)
    return false;
  if ((x_loc_==test.x_loc_) && (y_loc_==test.y_loc_) && (z_loc_==test.z_loc_))
    return true;
  else
    return false;
}

template <class T>
boct_loc_code<T> * boct_loc_code<T>::XOR(boct_loc_code<T> * b)
{
    boct_loc_code<T> * xorcode=new boct_loc_code<T>();
    xorcode->x_loc_=this->x_loc_^b->x_loc_;
    xorcode->y_loc_=this->y_loc_^b->y_loc_;
    xorcode->z_loc_=this->z_loc_^b->z_loc_;

    return xorcode;
}

//: returns the AND of the locational codes of this and b
template <class T>
boct_loc_code<T> * boct_loc_code<T>::AND(boct_loc_code *b)
{
  boct_loc_code<T> * xorcode=new boct_loc_code<T>();
  xorcode->x_loc_=this->x_loc_&b->x_loc_;
  xorcode->y_loc_=this->y_loc_&b->y_loc_;
  xorcode->z_loc_=this->z_loc_&b->z_loc_;

  return xorcode;
}

//: function to convert locational code to a point.
template <class T>
vgl_point_3d<double> boct_loc_code<T>::get_point(short max_level)
{
    int max_val = 1 << (max_level-1);

    vgl_point_3d<double> p((double)x_loc_/(double)(max_val),
                           (double)y_loc_/(double)(max_val),
                           (double)z_loc_/(double)(max_val));

    return p;
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, const boct_loc_code<T>& c)
{
  constexpr short io_version_no = 1;

  vsl_b_write(os, io_version_no);
  vsl_b_write(os, c.x_loc_);
  vsl_b_write(os, c.y_loc_);
  vsl_b_write(os, c.z_loc_);
  vsl_b_write(os, c.level);
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boct_loc_code<T>& c)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    short x, y, z,level;
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    vsl_b_read(is, z);
    vsl_b_read(is, level);
    c.set_code(x,y,z);
    c.set_level(level);
    break;

  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boct_loc_code&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

#define BOCT_LOC_CODE_INSTANTIATE(T) \
template class boct_loc_code<T >; \
template void vsl_b_write(vsl_b_ostream & os,const boct_loc_code<T >&); \
template void vsl_b_read(vsl_b_istream & is,  boct_loc_code<T >&); \
template std::ostream& operator <<(std::ostream &s, boct_loc_code<T >& )

#endif
