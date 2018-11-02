// This is brl/bseg/bvpl/bvpl_octree/bvpl_octree_vrml_util.h
#ifndef bvpl_octree_vrml_util_h
#define bvpl_octree_vrml_util_h
//:
// \file
// \brief VRML utilities for scenes with bvpl_octree samples
// \author Isabel Restrepo mir@lems.brown.edu
// \date  December 10,  2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/bvpl_octree/bvpl_cell_data_traits.h>

#include <boxm/boxm_scene.h>
#include <bvrml/bvrml_write.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvpl_octree_vrml_util
{
 public:
   //: Writes direction of kernel and magnitude of the response as disks of different sizes
  template<class T_data>
  static void write_scene_as_disks(std::ofstream &vrml_os,
                                   boxm_scene<boct_tree<short, bvpl_octree_sample<T_data> > >  *scene,
                                   bvpl_kernel_vector_sptr kernel_vector);
};

template<class T_data>
void bvpl_octree_vrml_util::write_scene_as_disks(std::ofstream &vrml_os,
                                                 boxm_scene<boct_tree<short, bvpl_octree_sample<T_data> > > *scene,
                                                 bvpl_kernel_vector_sptr kernel_vector)
{
  typedef boct_tree<short, bvpl_octree_sample<T_data> > tree_type;
  typedef boct_tree_cell<short, bvpl_octree_sample<T_data> > cell_type;

  bvrml_write::write_vrml_header(vrml_os);

  // iterate through the scene
  boxm_block_iterator<tree_type > iter= scene->iterator();
  iter.begin();

  for (; !iter.end(); iter++)
  {
    scene->load_block(iter.index());
    tree_type  *tree = (*iter)->get_tree();

    // iterate through the tree
    std::vector<cell_type*> cells = tree->leaf_cells();
    unsigned lines = 0;
    float max_response =bvpl_cell_to_float(cells[0]);
    for (unsigned i = 0; i < cells.size(); i+=10)
    {
      int id = cells[i]->data().id_;
      if (id < 0)
        continue;
      bvpl_kernel_sptr kernel = kernel_vector->kernels_[id];
      float response = bvpl_cell_to_float(cells[i]);
      if (response > max_response) {
        max_response = response;
      }
      std::cout << "response " << response << std::endl;
      vgl_vector_3d<double> dir((kernel->axis())[0],(kernel->axis())[1],(kernel->axis())[2]);
      vgl_point_3d<double> coord = tree->global_origin(cells[i]);
      if (response > 3.5f) {
        bvrml_write::write_vrml_line(vrml_os, coord, dir, .1f*response, 1, 0, 0);
        lines++;
      }
#ifdef DEBUG
      bvrml_write::write_vrml_disk(vrml_os, coord, dir, 2*response-.5f, 0, .5f, 0);
#endif
    }
    std::cout << "max response " << max_response << std::endl
             << "number of lines: " << lines << std::endl;
  }
}
#endif
