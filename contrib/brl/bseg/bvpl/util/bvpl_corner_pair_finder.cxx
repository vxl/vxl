#include "bvpl_corner_pair_finder.h"
#include <bvpl/bvpl_subgrid_iterator.h>
#include <vnl/vnl_math.h>

bvpl_corner_pairs_sptr bvpl_corner_pair_finder::find_pairs(bvxm_voxel_grid<int>* id_grid, bvpl_kernel_vector_sptr search_kernels,
                                                           bvpl_kernel_vector_sptr corner_kernels)
{
  //sanity check
  if (search_kernels->size() != corner_kernels->size()){
    vcl_cerr << "Error: kernels have different size()" << vcl_endl;
    return NULL;
  }

  vcl_vector<vcl_vector<vgl_line_segment_3d<int> > > all_lines;

  //1. run the same direction(id) kernel as the one we are intersted in
  for (int target_id=0; target_id<search_kernels->size(); ++target_id)
  {
    bvpl_kernel_sptr this_search_kernel = search_kernels->kernels_[target_id];

    bvpl_kernel_sptr target_corner_kernel = corner_kernels->kernels_[target_id];

    vcl_cout << "Search kernel " << target_id << " with size" << this_search_kernel->max() << ',' << this_search_kernel->min()<<vcl_endl;

    /*******************************************************************/
    bvpl_subgrid_iterator<int> id_subgrid_iter(id_grid, this_search_kernel->max(), this_search_kernel->min());
    vcl_vector<vgl_line_segment_3d<int> > lines;

    while (!id_subgrid_iter.isDone())
    {
      bvpl_kernel_iterator kernel_iter = this_search_kernel->iterator();
      bvpl_voxel_subgrid<int> id_subgrid = *id_subgrid_iter;
      vgl_point_3d<int> p1 = id_subgrid_iter.global_cur_voxel();
      int id_at_center = id_subgrid.get_voxel();
      if (id_at_center == target_id) {
        vcl_cout << p1 << " id " << id_at_center << vcl_endl;
        //reset the iterator
        kernel_iter.begin();
        while (!kernel_iter.isDone()) {
          vgl_point_3d<int> idx = kernel_iter.index();
          int this_id=-1;
          if (id_subgrid.voxel(idx, this_id)) {
            if (this_id >=0) {
              bvpl_kernel_sptr this_corner_kernel = corner_kernels->kernels_[this_id];
              if (this_corner_kernel->axis()== target_corner_kernel->axis()){//same plane now look for opossite diagonal corners
                if (vcl_abs(vcl_abs(this_corner_kernel->angle() - target_corner_kernel->angle()) - vnl_math::pi_over_2) < 1e-7) {
                  vgl_point_3d<int> p2(p1.x()+idx.x(), p1.y()+idx.y(), p1.z()-idx.z());
                  lines.push_back(vgl_line_segment_3d<int>(p1, p2));
                }
              }
            }
          }
          ++kernel_iter;
        }
      }
      ++id_subgrid_iter;
    }
    all_lines.push_back(lines);
  }

  return new bvpl_corner_pairs(all_lines);
}
