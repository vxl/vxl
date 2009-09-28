#ifndef bvpl_subgrid_iterator_h_
#define bvpl_subgrid_iterator_h_
//:
// \file
// \brief A class for iterating over the kernel values, paired with the position and value
//
// \author   Gamze tunali
// \date     May 28, 2009
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <vcl_iterator.h>
#include <vbl/vbl_ref_count.h>

#include <vgl/vgl_vector_3d.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include "bvpl_voxel_subgrid.h"


//: The base class for the iterators.
class bvpl_subgrid_iterator_base : public vbl_ref_count
{
 public:
  bvpl_subgrid_iterator_base() {}
  
  bvpl_subgrid_iterator_base(vgl_point_3d<int> max,  vgl_point_3d<int> min){
    
    max_=max;
    min_=min; 
    
    int x,y,z;
    x=(min.x() > 0)? max.x()+1:max.x()-min.x()+1;
    y=(min.y() > 0)? max.y()+1:max.y()-min.y()+1;
    z= max.z() - min.z() +1;   //no need to check min this coordinate is inverted
  
    dim_ = vgl_vector_3d<int>(x,y,z);
    
    x= (min.x() < 0)? -1*min.x():0;
    y= (min.y() < 0)? -1*min.y():0;
    z= (max.z() > 0)? max.z():0;
    
    offset_ = vgl_point_3d<int>(x,y,z);
    
  }
  ~bvpl_subgrid_iterator_base() {}

 protected:
  vgl_vector_3d<int> dim_;
  vgl_point_3d<int> offset_;
  vgl_point_3d<int> max_;
  vgl_point_3d<int> min_;
  
};

template <class T>
class bvpl_subgrid_iterator : public bvpl_subgrid_iterator_base,
  public vcl_iterator<vcl_bidirectional_iterator_tag, T>
{
 public:
  bvpl_subgrid_iterator()
    : bvpl_subgrid_iterator_base() {}

  bvpl_subgrid_iterator(bvxm_voxel_grid<T>* grid, vgl_point_3d<int> max,  vgl_point_3d<int> min);

  ~bvpl_subgrid_iterator(){}

  //bvpl_subgrid_iterator<T>& operator=(const bvpl_subgrid_iterator<T>& that);

  //bool operator==(const bvpl_subgrid_iterator<T>& that);

  //bool operator!=(const bvpl_subgrid_iterator<T>& that);

  bvpl_subgrid_iterator<T>& operator++();
  //bvpl_subgrid_iterator<T>& operator+(unsigned const &rhs); // postfix version


  bvpl_subgrid_iterator<T>& operator--();
  //bvpl_subgrid_iterator& operator--(int); // postfix version

  bvpl_voxel_subgrid<T> operator*();

  bvpl_voxel_subgrid<T> operator->();

  void begin();
  
  bool isDone();
  
  vgl_point_3d<int> global_cur_voxel(){ return global_cur_voxel_;  }

  vgl_point_3d<int> cur_voxel(){return cur_voxel_;}
 private:
  bvxm_voxel_grid<T>* grid_;
  //the z-coordinate of cur_voxel_ is always with respect to the local slab chunck
  vgl_point_3d<int> cur_voxel_; 
  //the z-coordinate of global_cur_voxel_ is with respect to the grid
  vgl_point_3d<int> global_cur_voxel_;
  bvxm_voxel_slab_iterator<T> iter_;
};

typedef vbl_smart_ptr<bvpl_subgrid_iterator_base> bvpl_subgrid_iterator_base_sptr;

#endif
