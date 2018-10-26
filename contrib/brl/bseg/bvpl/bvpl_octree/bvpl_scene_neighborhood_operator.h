// This is brl/bseg/bvpl/bvpl_octree/bvpl_scene_neighborhood_operator.h
#ifndef bvpl_scene_neighborhood_operator_h
#define bvpl_scene_neighborhood_operator_h
//:
// \file
// \brief A class to perform neighborhood operations on a boxm_scene.
// \author Isabel Restrepo mir@lems.brown.edu
// \date  13-Aug-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/boxm_scene.h>

#include <vgl/vgl_box_3d.h>

//: A class to perform neighborhood operations on a bvxm_scene.
// This class operates on all leaf cells within a region
class bvpl_scene_neighborhood_operator
{
 public:
  bvpl_scene_neighborhood_operator()= default;

  //: Applies the region-based functor on every leaf cell of the scene. The output is stored on a separate scene
  template <class T_data_in, class T_data_out, class F>
  void operate(boxm_scene<boct_tree<short, T_data_in > > *scene_in,
               F functor,
               vgl_box_3d<double> roi,
               boxm_scene<boct_tree<short, T_data_out > > *scene_out);

  //: Applies the region-based functor on every leaf cell of the scene. The region is obtained from scen2.
  //  The output is stored on a separate scene
  template <class T_data_in1, class T_data_in2, class T_data_out, class F>
  void operate(boxm_scene<boct_tree<short, T_data_in1 > > *scene_in1,
               boxm_scene<boct_tree<short, T_data_in2 > > *scene_in2,
               F functor,
               vgl_box_3d<double> roi,
               boxm_scene<boct_tree<short, T_data_out > > *scene_out);

  //: Applies a non-maxima suppression functor on every leaf cell of the scene. The output is stored in situ
  template <class T_data, class F>
  void local_non_maxima_suppression(boxm_scene<boct_tree<short, T_data > > &scene_in,
                                    F functor,
                                    vgl_box_3d<double> &roi);
};


//: Applies the region-based functor on every leaf cell of the scene. The output is stored in situ. The ROI must be in scene-coordinates
//(1)Traverse input scene and for every leaf cell, (2) request a region around it, and (3) apply the functor
template <class T_data, class F>
void bvpl_scene_neighborhood_operator::local_non_maxima_suppression(boxm_scene<boct_tree<short, T_data > > &scene_in,
                                                                    F functor,vgl_box_3d<double> &roi)
{
  short finest_level = scene_in.finest_level();

  //(1) Traverse the scene
  boxm_cell_iterator<boct_tree<short, T_data > > iterator = scene_in.cell_iterator(&boxm_scene<boct_tree<short, T_data> >::load_block_and_neighbors);

  iterator.begin();

  while (!iterator.end())
  {
    boct_tree_cell<short,T_data> *cell = *iterator;
    if ((!(cell->level() == finest_level)) || !cell->is_leaf()){
      ++iterator;
      continue;
    }
    // the region is located with subvoxel accuracy so datastructure should contain subvoxel localization
    roi.set_centroid((cell->data()).location());

    T_data this_data= cell->data();
    functor.init(this_data, roi);

    // get all cells that intersect a region
    std::vector<boct_tree_cell<short, T_data>* > cells_in_roi;

    //Return all cells that intersec the region
    scene_in.leaves_in_region(roi,cells_in_roi);

    for (unsigned i = 0; i<cells_in_roi.size(); i++)
    {
      boct_tree_cell<short, T_data> *this_cell = cells_in_roi[i];
      if (!this_cell)
        continue;
      T_data val = this_cell->data();
      functor.apply(val);
    }

    cell->set_data(functor.result());
    ++iterator;
  }
}


#endif
