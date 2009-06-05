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
//   
// \endverbatim

#include <vcl_iterator.h>
#include <vbl/vbl_ref_count.h>

#include <vgl/vgl_vector_3d.h>
#include <bvxm/bvxm_voxel_grid.h>
#include "bvpl_voxel_subgrid.h"


//: The base class for the iterators.
class bvpl_subgrid_iterator_base : public vbl_ref_count
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

  bvpl_subgrid_iterator(bvxm_voxel_grid<T>* grid, vgl_vector_3d<int> dimensions);

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

private:
  bvxm_voxel_grid<T>* grid_;
  vgl_point_3d<int> cur_voxel_;
  bvxm_voxel_slab_iterator<T> iter_;
};

typedef vbl_smart_ptr<bvpl_subgrid_iterator_base> bvpl_subgrid_iterator_base_sptr;

#endif