#include "bugl_curve_3d.h"
#include <bugl/bugl_normal_point_3d_sptr.h>
#include <vcl_cassert.h>

bugl_normal_point_3d_sptr bugl_curve_3d::get_point(unsigned int index) const
{
  assert(index < data_.size());
  return data_[index][num_neighbors_];
}

vcl_vector<bugl_normal_point_3d_sptr> bugl_curve_3d::get_neighbors(unsigned int index) const
{
  assert(index < data_.size());
  return data_[index];
}

bugl_normal_point_3d_sptr bugl_curve_3d::get_neighbor(unsigned int self, int offset) const
{
  assert(offset > -int(num_neighbors_) && offset < int(num_neighbors_));
  return data_[self][num_neighbors_ + offset];
}

void bugl_curve_3d::add_curve(vcl_vector<bugl_normal_point_3d_sptr > & pts)
{
  unsigned int size = pts.size();
  assert(size > 2*num_neighbors_ + 1);

  int prev_total = data_.size();

  vcl_vector<bugl_normal_point_3d_sptr> seg(2*num_neighbors_+1);
  for (unsigned int i=0; i<size; i++){
    seg[num_neighbors_] = pts[i]; // assign the middle point
    for (unsigned int j=1; j<=num_neighbors_; j++){
      // assign the left neighbors
      if (j > i)
        seg[num_neighbors_-j] = 0;
      else
        seg[num_neighbors_-j] = pts[i-j];

      // assign the right neighbors
      if (i+j < size)
        seg[num_neighbors_ + j] = pts[i+j];
      else
        seg[num_neighbors_ + j] = 0;
    }//end neighbors

    data_.push_back(seg);
  }//end of all the points

  index_.push_back(prev_total);
}

unsigned int bugl_curve_3d::get_fragment_size(unsigned int i) const
{
  if (i+1<index_.size())
    return index_[i+1] - index_[i];
  else 
    return data_.size() - index_[i];
}
