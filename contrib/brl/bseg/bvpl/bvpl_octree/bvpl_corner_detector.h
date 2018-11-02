// This is brl/bseg/bvpl/bvpl_octree/bvpl_corner_detector.h
#ifndef bvpl_corner_detector_h
#define bvpl_corner_detector_h
//:
// \file
// \brief A class to detect 3d Harris corners, based on the 2nd moment matrix
// \author Isabel Restrepo mir@lems.brown.edu
// \date  20-Jul-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_determinant.h>

class bvpl_corner_detector
{
 public:
  //: Detects Harris features  kernel with an input octree
  //  However, features below the Harris threshold are set as invalid in the valid_scene (in place)
  //  The type of measure used, depends on the functor.
  template<class T_data, class F>
  void detect_and_threshold( boxm_scene<boct_tree<short, T_data > > *scene_in,
                             F harris_functor,
                             vgl_point_3d<int> min_neigborhood_idx,
                             vgl_point_3d<int> max_neigborhood_idx,
                             unsigned block_i, unsigned block_j, unsigned block_k,
                             boxm_scene<boct_tree<short, bool> > *valid_scene,
                             boxm_scene<boct_tree<short, float> > *corner_scene,
                             double cell_length);

  //: Computes the corner measure and stores it in "corner_scene"
  //  Input scene and auxiliary valid scene are not modified
  template<class T_data, class F>
  void compute_C( boxm_scene<boct_tree<short, T_data > > *scene_in,
                  F harris_functor,
                  vgl_point_3d<int> min_neigborhood_idx,
                  vgl_point_3d<int> max_neigborhood_idx,
                  unsigned block_i, unsigned block_j, unsigned block_k,
                  boxm_scene<boct_tree<short, bool> > *valid_scene,
                  boxm_scene<boct_tree<short, float> > *corner_scene,
                  double cell_length);

  //: Compute the determinant of the hessian
  template<class T_data>
  void compute_det_H(boxm_scene<boct_tree<short, T_data > > *scene_in,
                     unsigned block_i, unsigned block_j, unsigned block_k,
                     boxm_scene<boct_tree<short, bool> > *valid_scene,
                     boxm_scene<boct_tree<short, float> > *corner_scene);
};


template<class T_data, class F>
void bvpl_corner_detector::detect_and_threshold( boxm_scene<boct_tree<short, T_data > > *scene_in,
                                                 F harris_functor,
                                                 vgl_point_3d<int> min_neigborhood_idx,
                                                 vgl_point_3d<int> max_neigborhood_idx,
                                                 unsigned block_i, unsigned block_j, unsigned block_k,
                                                 boxm_scene<boct_tree<short, bool> > *valid_scene,
                                                 boxm_scene<boct_tree<short, float> > *corner_scene,
                                                 double cell_length)
{
  typedef boct_tree<short, T_data> tree_type;
  typedef boct_tree_cell<short,T_data> cell_type;

  std::cout << "bvpl_block_kernel_operator: Operating on cells of length: " << cell_length << std::endl;

  // Load input and output blocks
  scene_in->load_block_and_neighbors(block_i,block_j,block_k);
  valid_scene->load_block(block_i,block_j,block_k);
  corner_scene->load_block(block_i,block_j,block_k);

  tree_type *tree_in = scene_in->get_block(block_i, block_j, block_k)->get_tree();
  boct_tree<short, bool> *valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
  boct_tree<short, float> *corner_tree = tree_in->template clone_to_type<float>();
  corner_tree->init_cells(0.0f);

  std::vector<cell_type* > cells_in = tree_in->leaf_cells();
  std::vector<boct_tree_cell<short, bool> * > valid_cells = valid_tree->leaf_cells();
  std::vector<boct_tree_cell<short, float> * > corner_cells = corner_tree->leaf_cells();

  //iterators
  typename std::vector<cell_type* >::iterator it_in = cells_in.begin();
  std::vector<boct_tree_cell<short, bool> * >::iterator valid_it = valid_cells.begin();
  std::vector<boct_tree_cell<short, float> * >::iterator corner_it = corner_cells.begin();

  unsigned long n_fail = 0;

  for (; (it_in!=cells_in.end())&& (valid_it!=valid_cells.end()) && (corner_it!=corner_cells.end()); it_in++, valid_it++, corner_it++)
  {
    boct_tree_cell<short,T_data> *center_cell = *it_in;
    boct_tree_cell<short, bool> *valid_center_cell = *valid_it;
    boct_tree_cell<short, float> *corner_center_cell = *corner_it;

    if (!valid_center_cell->data()) {
      continue;
    }
#if 0
    boct_loc_code<short> in_code = center_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((center_cell->level() != out_center_cell->level()) || !(in_code.isequal(&out_code))) {
      std::cerr << " Input and output cells don't have the same structure\n";
      continue;
    }
#endif

    vgl_point_3d<double> center_cell_centroid = tree_in->global_centroid(center_cell);

    for (int k = min_neigborhood_idx.z(); k <= max_neigborhood_idx.z(); k++)
      for (int j = min_neigborhood_idx.y(); j <= max_neigborhood_idx.y(); j++)
        for (int i = min_neigborhood_idx.x(); i <= max_neigborhood_idx.x(); i++)
        {
          vgl_point_3d<int> neighbor_cell_idx(i, j, k);

          vgl_point_3d<double> neighbor_cell_centroid(center_cell_centroid.x() + ((double)neighbor_cell_idx.x())*cell_length,
                                                      center_cell_centroid.y() + ((double)neighbor_cell_idx.y())*cell_length,
                                                      center_cell_centroid.z() + ((double)neighbor_cell_idx.z())*cell_length);

          boct_tree_cell<short,T_data> *this_cell = scene_in->locate_point_in_memory(neighbor_cell_centroid);

          if (this_cell) {
            harris_functor.apply(this_cell->data());
          }
          else
            break;
        }

    double C = 0;
    if (!harris_functor.result(C)) {
      valid_center_cell->set_data(false);
      ++n_fail;
    }
    corner_center_cell->set_data(float(C));
  }

  std::cout << "Number of Harris fails: " << n_fail << std::endl;
  //write the output block
  valid_scene->get_block(block_i, block_j, block_k)->init_tree(valid_tree);
  valid_scene->write_active_block();
  corner_scene->get_block(block_i, block_j, block_k)->init_tree(corner_tree);
  corner_scene->write_active_block();
}

template<class T_data, class F>
void bvpl_corner_detector::compute_C( boxm_scene<boct_tree<short, T_data > > *scene_in,
                                      F harris_functor,
                                      vgl_point_3d<int> min_neigborhood_idx,
                                      vgl_point_3d<int> max_neigborhood_idx,
                                      unsigned block_i, unsigned block_j, unsigned block_k,
                                      boxm_scene<boct_tree<short, bool> > *valid_scene,
                                      boxm_scene<boct_tree<short, float> > *corner_scene,
                                      double cell_length)
{
  typedef boct_tree<short, T_data> tree_type;
  typedef boct_tree_cell<short,T_data> cell_type;

  std::cout << "bvpl_block_kernel_operator: Operating on cells of length: " << cell_length << std::endl;

  // Load input and output blocks
  scene_in->load_block_and_neighbors(block_i,block_j,block_k);
  valid_scene->load_block(block_i,block_j,block_k);
  corner_scene->load_block(block_i,block_j,block_k);

  tree_type *tree_in = scene_in->get_block(block_i, block_j, block_k)->get_tree();
  boct_tree<short, bool> *valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
  boct_tree<short, float> *corner_tree = tree_in->template clone_to_type<float>();
  corner_tree->init_cells(0.0f);

  std::vector<cell_type* > cells_in = tree_in->leaf_cells();
  std::vector<boct_tree_cell<short, bool> * > valid_cells = valid_tree->leaf_cells();
  std::vector<boct_tree_cell<short, float> * > corner_cells = corner_tree->leaf_cells();

  //iterators
  typename std::vector<cell_type* >::iterator it_in = cells_in.begin();
  std::vector<boct_tree_cell<short, bool> * >::iterator valid_it = valid_cells.begin();
  std::vector<boct_tree_cell<short, float> * >::iterator corner_it = corner_cells.begin();

  unsigned long n_fail = 0;

  for (; (it_in!=cells_in.end())&& (valid_it!=valid_cells.end()) && (corner_it!=corner_cells.end()); it_in++, valid_it++, corner_it++)
  {
    boct_tree_cell<short,T_data> *center_cell = *it_in;
    boct_tree_cell<short, bool> *valid_center_cell = *valid_it;
    boct_tree_cell<short, float> *corner_center_cell = *corner_it;

    if (!valid_center_cell->data()) {
      continue;
    }
#if 0
    boct_loc_code<short> in_code = center_cell->get_code();

    //if level and location code of cells isn't the same then continue
    if ((center_cell->level() != out_center_cell->level()) || !(in_code.isequal(&out_code))) {
      std::cerr << " Input and output cells don't have the same structure\n";
      continue;
    }
#endif

    vgl_point_3d<double> center_cell_centroid = tree_in->global_centroid(center_cell);

    for (int k = min_neigborhood_idx.z(); k <= max_neigborhood_idx.z(); k++)
      for (int j = min_neigborhood_idx.y(); j <= max_neigborhood_idx.y(); j++)
        for (int i = min_neigborhood_idx.x(); i <= max_neigborhood_idx.x(); i++)
        {
          vgl_point_3d<int> neighbor_cell_idx(i, j, k);

          vgl_point_3d<double> neighbor_cell_centroid(center_cell_centroid.x() + ((double)neighbor_cell_idx.x())*cell_length,
                                                      center_cell_centroid.y() + ((double)neighbor_cell_idx.y())*cell_length,
                                                      center_cell_centroid.z() + ((double)neighbor_cell_idx.z())*cell_length);

          boct_tree_cell<short,T_data> *this_cell = scene_in->locate_point_in_memory(neighbor_cell_centroid);

          if (this_cell) {
            harris_functor.apply(this_cell->data());
          }
          else
            break;
        }

    double C = 0;
    harris_functor.result(C);
    corner_center_cell->set_data(float(C));
  }

  std::cout << "Number of Harris fails: " << n_fail << std::endl;
  //write the output block
  corner_scene->get_block(block_i, block_j, block_k)->init_tree(corner_tree);
  corner_scene->write_active_block();
}


template<class T_data>
void bvpl_corner_detector::compute_det_H( boxm_scene<boct_tree<short, T_data > > *scene_in,
                                          unsigned block_i, unsigned block_j, unsigned block_k,
                                          boxm_scene<boct_tree<short, bool> > *valid_scene,
                                          boxm_scene<boct_tree<short, float> > *corner_scene)
{
  typedef boct_tree<short, T_data> tree_type;
  typedef boct_tree_cell<short,T_data> cell_type;

  // Load input and output blocks
  scene_in->load_block_and_neighbors(block_i,block_j,block_k);
  valid_scene->load_block(block_i,block_j,block_k);
  corner_scene->load_block(block_i,block_j,block_k);

  tree_type *tree_in = scene_in->get_block(block_i, block_j, block_k)->get_tree();
  boct_tree<short, bool> *valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
  boct_tree<short, float> *corner_tree = tree_in->template clone_to_type<float>();
  corner_tree->init_cells(0.0f);

  std::vector<cell_type* > cells_in = tree_in->leaf_cells();
  std::vector<boct_tree_cell<short, bool> * > valid_cells = valid_tree->leaf_cells();
  std::vector<boct_tree_cell<short, float> * > corner_cells = corner_tree->leaf_cells();

  //iterators
  typename std::vector<cell_type* >::iterator it_in = cells_in.begin();
  std::vector<boct_tree_cell<short, bool> * >::iterator valid_it = valid_cells.begin();
  std::vector<boct_tree_cell<short, float> * >::iterator corner_it = corner_cells.begin();

  for (; (it_in!=cells_in.end())&& (valid_it!=valid_cells.end()) && (corner_it!=corner_cells.end()); it_in++, valid_it++, corner_it++)
  {
    boct_tree_cell<short,T_data> *center_cell = *it_in;
    boct_tree_cell<short, bool> *valid_center_cell = *valid_it;
    boct_tree_cell<short, float> *corner_center_cell = *corner_it;

    if (!valid_center_cell->data()) {
      continue;
    }

    vnl_vector_fixed<double,10> mean_coeff = center_cell->data();

    vnl_double_3x3 H;
    H.put(0,0,mean_coeff[4]);
    H.put(0,1,mean_coeff[7]);
    H.put(0,2,mean_coeff[8]);
    H.put(1,0,mean_coeff[7]);
    H.put(1,1,mean_coeff[5]);
    H.put(1,2,mean_coeff[9]);
    H.put(2,0,mean_coeff[8]);
    H.put(2,1,mean_coeff[9]);
    H.put(2,2,mean_coeff[6]);

    corner_center_cell->set_data(float(vnl_determinant(H)));
  }

  //write the output block
  corner_scene->get_block(block_i, block_j, block_k)->init_tree(corner_tree);
  corner_scene->write_active_block();
}

#endif
