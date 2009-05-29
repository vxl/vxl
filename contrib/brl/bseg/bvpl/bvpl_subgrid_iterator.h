#ifndef bvpl_subgrid_iterator_h_
#define bvpl_subgrid_iterator_h_
//:
// \file

#include <vcl_iterator.h>

#include <vgl/vgl_vector_3d.h>
#include <bvxm/bvxm_voxel_grid.h>
#include "bvpl_voxel_subgrid.h"


//: The base class for the iterators.
class bvpl_subgrid_iterator_base
{
 public:
   bvpl_subgrid_iterator_base() {}
   bvpl_subgrid_iterator_base(vgl_vector_3d<int> dimensions) : dim_(dimensions) {}
  ~bvpl_subgrid_iterator_base() {}

 protected:
  vgl_vector_3d<int> dim_;
};

template <class T>
class bvpl_subgrid_iterator : public bvpl_subgrid_iterator_base,
  public vcl_iterator<vcl_bidirectional_iterator_tag, T>
{
 public:
  bvpl_subgrid_iterator()
    : bvpl_subgrid_iterator_base() {}

  bvpl_subgrid_iterator(bvxm_voxel_grid<T>& grid, vgl_vector_3d<int> dimensions);

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

  bool done();
private:
  bvxm_voxel_grid<T> grid_;
  vgl_point_3d<int> cur_voxel_;
  bvxm_voxel_slab_iterator<T> iter_;
};

#endif