#include "bugl_curve_3d.h"
#include <bbas/bugl/bugl_normal_point_3d_sptr.h>
#include <bbas/bugl/bugl_normal_point_2d_sptr.h>
#include <bugl/bugl_normal_point_2d.h>
#include <bugl/bugl_normal_point_3d.h>
#include <vcl_iostream.h>

bugl_curve_3d::~bugl_curve_3d()
{

}

bugl_curve_3d::bugl_curve_3d(const int n)
{
	num_neighbors_=n;
}

bugl_normal_point_3d_sptr bugl_curve_3d::get_point(const int index) const
{
	return data_[index][num_neighbors_];
}

vcl_vector<bugl_normal_point_3d_sptr> bugl_curve_3d::get_neighbors(const int index) const
{
  return data_[index];
}

bugl_normal_point_3d_sptr bugl_curve_3d::get_neighbor(const int self, const int offset ) const
{
  assert(offset > -num_neighbors_ && offset < num_neighbors_);
  return data_[self][num_neighbors_ + offset];
}

int bugl_curve_3d::add_curve(vcl_vector<bugl_normal_point_3d_sptr > & pts)
{
  int size = pts.size();
  assert(size > 2*num_neighbors_ + 1);

  int prev_total = data_.size();

  vcl_vector<bugl_normal_point_3d_sptr> seg(2*num_neighbors_+1);
  for(int i=0; i<size; i++){
    seg[num_neighbors_] = pts[i]; // assign the middle point
    for(int j=1; j<=num_neighbors_; j++){
      // assign the left neighbors
      if(j > i)
        seg[num_neighbors_-j] = 0;
      else
        seg[num_neighbors_-j] = pts[i-j];

      // assign the right neighbors
      if(j < size - i)
        seg[num_neighbors_ + j] = pts[i+j];
      else
        seg[num_neighbors_ + j] = 0;
    }//end neighbors

    data_.push_back(seg);
      
  }//end of all the points

  index_.push_back(prev_total);

  return 0;
}

int bugl_curve_3d::get_num_points() const
{
  return data_.size();
}

int bugl_curve_3d::get_num_fragments() const
{
  return index_.size();
}

int bugl_curve_3d::get_fragment_size(int i) const
{
  if(i<index_.size()-1)
    return index_[i+1] - index_[i];
  else 
    return data_.size() - index_[i];
}
