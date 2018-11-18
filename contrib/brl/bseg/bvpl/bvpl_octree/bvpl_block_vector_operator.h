// This is brl/bseg/bvpl/bvpl_octree/bvpl_block_vector_operator.h
#ifndef bvpl_block_vector_operator_h
#define bvpl_block_vector_operator_h

//:
// \file
// \brief A class operates a kernel on a boxm_block
// \author Isabel Restrepo mir@lems.brown.edu
// \date  1-Feb-2011.
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

//:A class operates a kernel on a boxm_block.
// This class differes from bvpl_octree_kernel_operator in that it loads neighbor block to process blocks appropriately.
// This class is useful to run multathreding processes
class bvpl_block_vector_operator
{
 public:
  //: "Convolves" kernel with an input octree, storing the output in an output octree.
  //  This function only iterate through leaf_cells at level 0;
  template<class T_data_in, class T_data_out,class F, unsigned dim >
  void operate(boxm_scene<boct_tree<short, T_data_in > > &scene_in,
               F functor,
               bvpl_kernel_vector_sptr kernel_vector,
               unsigned block_i, unsigned block_j, unsigned block_k,
               boxm_scene<boct_tree<short, vnl_vector_fixed<T_data_out, dim> > > &scene_out,
               boxm_scene<boct_tree<short, bool> > &valid_scene, double cell_length);
};

//: Operates a kernel on a scene by : (1)Traverse input scene and for every leaf cell, (2) request a region around it, and (3) apply the functor
template<class T_data_in, class T_data_out,class F, unsigned dim >
void bvpl_block_vector_operator::operate(boxm_scene<boct_tree<short, T_data_in > > &scene_in,
                                         F functor,
                                         bvpl_kernel_vector_sptr kernel_vector,
                                         unsigned block_i, unsigned block_j, unsigned block_k,
                                         boxm_scene<boct_tree<short, vnl_vector_fixed<T_data_out, dim> > > &scene_out,
                                         boxm_scene<boct_tree<short, bool> > &valid_scene, double cell_length)
{
  if (kernel_vector->size()!=dim) {
    std::cout << "Error in bvpl_block_vector_operator::operate ---> size missmatch\n";
    return;
  }

  typedef boct_tree<short, T_data_in> tree_type_in;
  typedef boct_tree_cell<short,T_data_in> cell_type_in;

  typedef boct_tree<short, vnl_vector_fixed<T_data_out, dim> > tree_type_out;
  typedef boct_tree_cell<short, vnl_vector_fixed<T_data_out, dim> > cell_type_out;

  std::cout << "bvpl_block_kernel_operator: Operating on cells of length: " << cell_length << std::endl;

  // Load input and output blocks
  scene_in.load_block_and_neighbors(block_i,block_j,block_k);
  scene_out.load_block(block_i,block_j,block_k);
  valid_scene.load_block(block_i,block_j,block_k);


  tree_type_in *tree_in = scene_in.get_block(block_i, block_j, block_k)->get_tree();
  tree_type_out *tree_out = tree_in->template clone_to_type<vnl_vector_fixed<T_data_out, dim> >();
  boct_tree<short, bool> *valid_tree = tree_in->template clone_to_type<bool>();

  std::vector<cell_type_in* > cells_in = tree_in->leaf_cells();
  std::vector<cell_type_out* > cells_out = tree_out->leaf_cells();
  std::vector<boct_tree_cell<short, bool> * > valid_cells = valid_tree->leaf_cells();

  //iterators
  typename std::vector<cell_type_in* >::iterator it_in = cells_in.begin();
  typename std::vector<cell_type_out* >::iterator it_out = cells_out.begin();
  typename std::vector<boct_tree_cell<short, bool> * >::iterator valid_it = valid_cells.begin();

  for (; (it_in!=cells_in.end())&&(it_out!= cells_out.end() && (valid_it!=valid_cells.end())); ++it_in, ++it_out, ++valid_it)
  {
    cell_type_in *center_cell = *it_in;
    cell_type_out *out_center_cell = *it_out;
    boct_tree_cell<short, bool> *valid_center_cell = *valid_it;
    bool valid = true;

#ifdef DEBUG
    boct_loc_code<short> out_code = out_center_cell->get_code();
    boct_loc_code<short> in_code = center_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((center_cell->level() != out_center_cell->level()) || !(in_code.isequal(&out_code))){
      std::cerr << " Input and output cells don't have the same structure\n";
      continue;
    }
#endif

    vgl_point_3d<double> center_cell_centroid = tree_in->global_centroid(center_cell);

    vnl_vector_fixed<T_data_out, dim> responses;

    for (unsigned k=0; k< kernel_vector->kernels_.size(); ++k)
    {
      bvpl_kernel_sptr kernel = kernel_vector->kernels_[k];
      bvpl_kernel_iterator kernel_iter = kernel->iterator();
      kernel_iter.begin(); // reset the kernel iterator
      while (!kernel_iter.isDone())
      {
        vgl_point_3d<int> kernel_idx = kernel_iter.index();

        vgl_point_3d<double> kernel_cell_centroid(center_cell_centroid.x() + (double)kernel_idx.x()*cell_length,
                                                  center_cell_centroid.y() + (double)kernel_idx.y()*cell_length,
                                                  center_cell_centroid.z() + (double)kernel_idx.z()*cell_length);

        boct_tree_cell<short,T_data_in> *this_cell = scene_in.locate_point_in_memory(kernel_cell_centroid);

        if (this_cell) {
          bvpl_kernel_dispatch d = *kernel_iter;
          T_data_in val =(T_data_in)this_cell->data();
          functor.apply(val, d);
        }
        else {
          valid = false;
          break;
        }

        ++kernel_iter;
      }
      responses[k] = (T_data_out)(functor.result());
    }

    out_center_cell->set_data(responses);
    valid_center_cell->set_data(valid);
  }

  //write the output block
  scene_out.get_block(block_i, block_j, block_k)->init_tree(tree_out);
  scene_out.write_active_block();
  valid_scene.get_block(block_i, block_j, block_k)->init_tree(valid_tree);
  valid_scene.write_active_block();
}

#endif // bvpl_block_vector_operator_h
