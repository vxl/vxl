#include "boct_loc_code.h"

boct_loc_code::boct_loc_code(const boct_loc_code& rhs)
{
  x_loc_ = rhs.x_loc_;
  y_loc_ = rhs.x_loc_;
  z_loc_ = rhs.x_loc_;
}
 
boct_loc_code::boct_loc_code(vgl_point_3d<double> p, short max_level)
{
  // root level r = n -1
  int max_val = 1 << (max_level-1);
  x_loc_ = (short) p.x()*max_level;
  y_loc_ = (short) p.y()*max_level;
  z_loc_ = (short) p.z()*max_level;

}

short boct_loc_code::child_index(short level)
{
  // level 0 cannot have a child, that ought to be the last level
  if (level == 0) 
    return -1;

  // the bits are stored as [00...00ZYX]
  short child_bit = 1 << (level-1);
  short index_x = (x_loc_ & child_bit) >> child_bit;
  short index_y = (y_loc_ & child_bit) >> (child_bit-1);
  short index_z = (z_loc_ & child_bit) >> (child_bit -2);
  return (index_x+index_y+index_z);
}

bool boct_loc_code::isequal(const boct_loc_code * test,short level)
{
  short relevantbit = 1 << (level-1);
  if( (x_loc_ & relevantbit) && (y_loc_ & relevantbit) && (z_loc_ & relevantbit) )
      return true;
  else
      return false;
}
