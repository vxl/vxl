// This is brl/bseg/bvpl/bvpl_octree/bvpl_scene_kernel_operator.h
#ifndef bvpl_scene_kernel_operator_h
#define bvpl_scene_kernel_operator_h
//:
// \file
// \brief A class that operates a bvpl_kernel on a boxm_scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 18, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/kernels/bvpl_kernel.h>
#include <boxm/boxm_scene.h>
#include "bvpl_octree_kernel_operator.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvpl_scene_kernel_operator
{
 public:
  //: "Convolves" kernel with an input octree, storing the output in an output octree.
  //  This function only iterate through leaf_cells at level 0;
  template<class T_data, class F>
  void operate(boxm_scene<boct_tree<short, T_data > > &scene_in,
               F functor,
               bvpl_kernel_sptr kernel,
               boxm_scene<boct_tree<short, T_data > > &scene_out);

  //: Performs in-situ non-maxima suppression over the area occupied by the kernel
  template<class T_data, class F>
  void local_non_maxima_suppression(boxm_scene<boct_tree<short, T_data > > &scene_in,
                                    F functor,
                                    bvpl_kernel_sptr kernel);

#if 0 //Depracated
  // "Convolves" kernel with an input octree, storing the output in an output octree.
  template<class T_data, class F>
  void operate(boxm_scene<boct_tree<short, T_data > > &scene_in,
               F functor,
               bvpl_kernel_sptr kernel,
               boxm_scene<boct_tree<short, T_data > > &scene_out,
               short level)
  {
    typedef boct_tree<short, T_data > tree_type;
    boxm_block_iterator<tree_type> iter_in = scene_in.iterator();
    boxm_block_iterator<tree_type> iter_out = scene_out.iterator();
    iter_in.begin();
    iter_out.begin();
    for (; !iter_in.end(); iter_in++, iter_out++) {
      //Isa: change to load block and neighbors
      scene_in.load_block(iter_in.index());
      scene_out.load_block(iter_out.index());
      tree_type *tree_in= (*iter_in)->get_tree();
      tree_type *tree_out = tree_in->clone();

      // output tree has the same shape but is initialized to have response zero
      T_data zero_val = T_data();
      std::cout << "Response tree is initialized to: " << zero_val << std::endl;
      tree_out->init_cells(zero_val);

      bvpl_octree_kernel_operator<T_data> oper(tree_in);
      double cell_length = 1.0/(double)(1<<(tree_in->root_level() -level));
      oper.operate(functor, kernel, tree_out, level, cell_length);
      (*iter_out)->init_tree(tree_out);
      scene_out.write_active_block();
    }
  }
#endif
};

//: Operates a kernel on a scene by : (1)Traverse input scene and for every leaf cell, (2) request a region around it, and (3) apply the functor
template<class T_data, class F>
void bvpl_scene_kernel_operator::operate(boxm_scene<boct_tree<short, T_data > > &scene_in,
                                         F functor,
                                         bvpl_kernel_sptr kernel,
                                         boxm_scene<boct_tree<short, T_data > > &scene_out)
{
  double cell_length = kernel->voxel_length();
  short finest_level = scene_in.finest_level();
  std::cout << "bvpl_scene_kernel_operator: Operating on cells at level: " << finest_level << " and length: " << cell_length << std::endl;

  scene_in.clone_blocks(scene_out, T_data());

  //(1) Traverse the scene - is there an easy way to modify the cell iterator so to only use leaf cells at level 0;
  boxm_cell_iterator<boct_tree<short, T_data > > iterator = scene_in.cell_iterator(&boxm_scene<boct_tree<short, T_data > >::load_block_and_neighbors);
  iterator.begin();

  boxm_cell_iterator<boct_tree<short, T_data > > out_iter = scene_out.cell_iterator(&boxm_scene<boct_tree<short, T_data > >::load_block);
  out_iter.begin();

  bvpl_kernel_iterator kernel_iter = kernel->iterator();

  while ( !(iterator.end() || out_iter.end()) )
  {
    boct_tree_cell<short,T_data> *center_cell = *iterator;
    boct_tree_cell<short,T_data> *out_center_cell = *out_iter;
    boct_loc_code<short> out_code = out_center_cell->get_code();
    boct_loc_code<short> in_code = center_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((center_cell->level() != out_center_cell->level()) || !(in_code.isequal(&out_code))){
      std::cerr << " Input and output cells don't have the same structure\n";
      ++iterator;
      ++out_iter;
      continue;
    }

    //we are only interested in finest resolution
    if ((!(center_cell->level() == finest_level)) || !center_cell->is_leaf()){
      ++iterator;
      ++out_iter;
      continue;
    }

    vgl_point_3d<double> center_cell_origin = iterator.global_origin();

    kernel_iter.begin(); // reset the kernel iterator
    while (!kernel_iter.isDone())
    {
      vgl_point_3d<int> kernel_idx = kernel_iter.index();

      vgl_point_3d<double> kernel_cell_origin(center_cell_origin.x() + (double)kernel_idx.x()*cell_length + 1.0e-7,
                                              center_cell_origin.y() + (double)kernel_idx.y()*cell_length + 1.0e-7,
                                              center_cell_origin.z() + (double)kernel_idx.z()*cell_length + 1.0e-7);

      boct_tree_cell<short,T_data> *this_cell = scene_in.locate_point_in_memory(kernel_cell_origin);

      if (this_cell) {
        bvpl_kernel_dispatch d = *kernel_iter;
        T_data val = this_cell->data();
        functor.apply(val, d);
      }
      else {
        break;
      }

      ++kernel_iter;
    }

    out_center_cell->set_data(functor.result());
    ++iterator;
    ++out_iter;
  }
}

//: Applies non-maxima suppression on the region occupied by the kernel. The output is stored in situ.
//(1)Traverse input scene and for every leaf cell, (2) request a region around it, and (3) apply the functor
template<class T_data, class F>
void bvpl_scene_kernel_operator::local_non_maxima_suppression(boxm_scene<boct_tree<short, T_data > > &scene_in,
                                                              F functor,
                                                              bvpl_kernel_sptr kernel)
{
  short finest_level = scene_in.finest_level();
  double cell_length = kernel->voxel_length();

  //(1) Traverse the scene - is there an easy way to modify the cell iterator so to only use leaf cells at level 0;
  boxm_cell_iterator<boct_tree<short, T_data > > iterator = scene_in.cell_iterator(&boxm_scene<boct_tree<short, T_data > >::load_block_and_neighbors);
  iterator.begin();

  bvpl_kernel_iterator kernel_iter = kernel->iterator();

  while ( !(iterator.end()) )
  {
    boct_tree_cell<short,T_data> *center_cell = *iterator;

    //we are only interested in finest resolution
    if (!center_cell->level() == finest_level || !center_cell->is_leaf()){
      ++iterator;
      continue;
    }
    T_data this_data = center_cell->data();

    //vgl_point_3d<double> center_cell_origin = iterator.global_origin();
    vgl_point_3d<double> center_cell_center = this_data.location();
    if (!functor.init(this_data)){
      ++iterator;
      continue;
    }

    kernel_iter.begin(); // reset the kernel iterator
    while (!kernel_iter.isDone())
    {
      vgl_point_3d<int> kernel_idx = kernel_iter.index();

      vgl_point_3d<double> kernel_cell_center(center_cell_center.x() + (double)kernel_idx.x()*cell_length,
                                              center_cell_center.y() + (double)kernel_idx.y()*cell_length,
                                              center_cell_center.z() + (double)kernel_idx.z()*cell_length);

      boct_tree_cell<short,T_data> *this_cell = scene_in.locate_point_in_memory(kernel_cell_center);

      if (this_cell) {
        //bvpl_kernel_dispatch d = *kernel_iter;
        T_data val = this_cell->data();
        functor.apply(val);
      }
      else {
        break;
      }

      ++kernel_iter;
    }

    center_cell->set_data(functor.result());
    ++iterator;
  }
}

#endif // bvpl_scene_kernel_operator_h
