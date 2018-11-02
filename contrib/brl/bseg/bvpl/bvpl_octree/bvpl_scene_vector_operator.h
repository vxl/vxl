// This is brl/bseg/bvpl/bvpl_octree/bvpl_scene_vector_operator.h
#ifndef bvpl_scene_vector_operator_h
#define bvpl_scene_vector_operator_h
//:
// \file
// \brief  A class that applies a vector of bvpl_kernels on a boxm_scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 30, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/functors/bvpl_local_max_functor.h>

#include <boxm/boxm_scene.h>

#include <vnl/vnl_vector_fixed.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvpl_scene_vector_operator
{
 public:

  //: "Convolves" a vector of kernel with an input octree, storing the output in an output octree.
  //  The shape of the output is determined by a templated function
  //  This function only iterate through leaf_cells at level 0;
  template<class T_data_in, class T_data_out, class F >
  void operate(boxm_scene<boct_tree<short, T_data_in> > &scene_in,
               F functor,
               bvpl_kernel_vector_sptr kernel_vector,
               boxm_scene<boct_tree<short, T_data_out > > &scene_out);

 protected:

  //:Function incharged of combining results
  template<class T_data>
  void combine_kernel_responses(const std::vector<T_data>& responses, vnl_vector_fixed<T_data, 3> &response_out);

  //:Function incharged of combining results
  template<class T_data>
  void combine_kernel_responses(const std::vector<T_data>& responses, bvpl_octree_sample<T_data> &response_out);
};


//: Operates a vector of kernels on a scene by : (1)Traversing input scene and for every leaf cell, (2) request a region around it, and (3) apply the functor
template<class T_data_in, class T_data_out,class F >
void bvpl_scene_vector_operator::operate(boxm_scene<boct_tree<short, T_data_in> > &scene_in,
                                         F functor,
                                         bvpl_kernel_vector_sptr kernel_vector,
                                         boxm_scene<boct_tree<short, T_data_out > > &scene_out)
{
  short finest_level = scene_in.finest_level();
  // CAUTION: kernel is run at finest resolution, but this may not always be desired
  double cell_length = scene_in.finest_cell_length();

  std::cout << "bvpl_scene_kernel_operator: Operating on cells at level: " << finest_level << " and length: " << cell_length << std::endl;

  T_data_out zero;
  scene_in.template clone_blocks_to_type<boct_tree<short, T_data_out > >(scene_out, zero);
  std::cout << "The initial zero-value for all cells is : " << zero << std::endl;

  //(1) Traverse the scene - is there an easy way to modify the cell iterator so to only use leaf cells at level 0;
  boxm_cell_iterator<boct_tree<short, T_data_in > > iterator = scene_in.cell_iterator(&boxm_scene<boct_tree<short, T_data_in > >::load_block_and_neighbors);
  iterator.begin();

  boxm_cell_iterator<boct_tree<short, T_data_out > > out_iter = scene_out.cell_iterator(&boxm_scene<boct_tree<short, T_data_out > >::load_block);
  out_iter.begin();

  while ( !(iterator.end() || out_iter.end()) )
  {
    boct_tree_cell<short,T_data_in> *center_cell = *iterator;
    boct_tree_cell<short,T_data_out> *out_center_cell = *out_iter;
    boct_loc_code<short> out_code = out_center_cell->get_code();
    boct_loc_code<short> in_code = center_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((center_cell->level() != out_center_cell->level()) || !(in_code.isequal(&out_code))) {
      std::cerr << " Input and output cells don't have the same structure " << std::endl;
      ++iterator;
      ++out_iter;
      continue;
    }

    //we are only interested in finest resolution
    if ((!(center_cell->level() == finest_level)) || !center_cell->is_leaf()) {
      ++iterator;
      ++out_iter;
      continue;
    }

    vgl_point_3d<double> center_cell_centroid = iterator.global_centroid();

    //(2) Run vector of kernels

    std::vector<T_data_in> responses;
    for (auto kernel : kernel_vector->kernels_)
    {
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
          T_data_in val = this_cell->data();
          functor.apply(val, d);
        }

        ++kernel_iter;
      }

      responses.push_back(functor.result());
    }

    T_data_out data_out;
    combine_kernel_responses(responses, data_out);
    out_center_cell->set_data(data_out);
    ++iterator;
    ++out_iter;
  }
  return;
}

template <class T_data>
void bvpl_scene_vector_operator::combine_kernel_responses(const std::vector<T_data>& responses, vnl_vector_fixed<T_data, 3> &response_out)
{
  if (responses.size() != 3) {
    std::cerr << "Wrong number of responses" << std::endl;
    return;
  }

  for (unsigned i = 0 ; i < 3; i++)
  {
    response_out[i] = responses[i];
  }
  return;
}

template <class T_data>
void bvpl_scene_vector_operator::combine_kernel_responses(const std::vector<T_data>& responses, bvpl_octree_sample<T_data> &response_out)
{
  if (responses.size() != 3) {
    std::cerr << "Wrong number of responses" << std::endl;
    return;
  }

  bvpl_local_max_functor<T_data> func_max;

  T_data max= responses[0];
  int max_id =0;
  for (unsigned i = 1 ; i < responses.size(); i++)
  {
    if (func_max.greater_than(max, responses[i])){
      max = responses[i];
      max_id = i;
    }
  }
  response_out.response_=max;
  response_out.id_ = max_id;
  return;
}

#endif // bvpl_scene_vector_operator_h
