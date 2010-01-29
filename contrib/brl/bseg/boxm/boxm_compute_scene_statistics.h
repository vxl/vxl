#ifndef boxm_compute_scene_statistics_h
#define boxm_compute_scene_statistics_h
#include <boxm/boxm_scene.h>
#include <vcl_iostream.h>
#include <bsta/bsta_histogram.h>

template <class T_loc, class T_data>
bool compute_scene_statistics(boxm_scene<boct_tree<T_loc, T_data > >& scene, bsta_histogram<float>& omega_hist, bsta_histogram<float>& sigma_hist, bsta_histogram<float>& level_hist, unsigned& n_leaves)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  typedef boct_tree_cell<T_loc,T_data> cell_type;

  omega_hist = bsta_histogram<float>(0.0f, 1.0f, 20);
  sigma_hist = bsta_histogram<float>(0.0f, 1.0f, 20);
  level_hist = bsta_histogram<float>(0.0f, 10.0f, 10);
  boxm_block_iterator<tree_type> bit = scene.iterator();
  for (;!bit.end(); ++bit)
  {
    vgl_point_3d<int> block_index = bit.index();
    scene.load_block(block_index);
    boxm_block<tree_type> * block = scene.get_active_block();
    tree_type* tree=block->get_tree();
    if (!tree) return false;
    int n_levels = tree->number_levels();
    int fin_level = tree->finest_level();
    for (int level = n_levels-1; level>=fin_level; --level)
    {
      vcl_vector<cell_type *> cells_at_level =
        tree->leaf_cells_at_level(static_cast<unsigned>(level));
      level_hist.upcount(static_cast<float>(level),
                         static_cast<float>(cells_at_level.size()));
    }
    vcl_vector<cell_type *> leaves = tree->leaf_cells();
    n_leaves = leaves.size();
    for (typename vcl_vector<cell_type *>::iterator cit = leaves.begin(); cit != leaves.end(); ++cit)
    {
      T_data data = (*cit)->data();
      vgl_box_3d<double> bb = tree->cell_bounding_box(*cit);
      double dx = bb.width(), dy = bb.height(), dz = bb.depth();
      double diag = vcl_sqrt(dx*dx + dy*dy + dz*dz);
      double alpha = data.alpha;
      double omega = 1.0 - vcl_exp(-alpha*diag);
      omega_hist.upcount(static_cast<float>(omega), 1.0f);

      if (scene.appearence_model() == BOXM_APM_SIMPLE_GREY)
      {
        float sigma = data.appearance_.sigma();
        sigma_hist.upcount(sigma, 1.0f);
      }
    }
  }
  return true;
}

#endif
