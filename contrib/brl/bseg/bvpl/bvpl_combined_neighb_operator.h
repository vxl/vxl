// This is//projects/vxl/src/contrib/brl/bseg/bvpl/bvpl_combined_neighb_operator.h
#ifndef bvpl_combined_neighb_operator_h
#define bvpl_combined_neighb_operator_h
//:
// \file
// \brief A class for performing simultaneous neighborhood operations on a appearance and occupancy grids.
// \author Isabel Restrepo mir@lems.brown.edu
// \date  September 20, 2009
//
// This class assumes occupancy=float and apperearance=unimodal gaussian. The class can be made more general
// in the future but at moment of creation it is not clear if it will be necessary.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_subgrid_iterator.h"
#include "bvpl_kernel_iterator.h"
#include "bvpl_voxel_subgrid.h"
#include "bvpl_kernel.h"

#include <vgl/vgl_point_3d.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_f1.h>


template <class OCP_FUNC, class APP_FUNC>
class bvpl_combined_neighb_operator
{
 public:
  bvpl_combined_neighb_operator(const OCP_FUNC ocp_functor, const APP_FUNC app_functor): ocp_func_(ocp_functor),app_func_(app_functor){}
  ~bvpl_combined_neighb_operator() {}

  // makes the neighborhood operation and stores the result in the output subgrid, which may be equal to the input
  void operate(bvxm_voxel_grid<float>* ocp_grid,
               bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> >* app_grid,
               bvpl_kernel_sptr kernel,
               bvxm_voxel_grid<float>* out_grid)
  {
    bvpl_subgrid_iterator<float> ocp_iter(ocp_grid, kernel->max(), kernel->min());
    bvpl_subgrid_iterator<bsta_num_obs<bsta_gauss_f1> > app_iter(app_grid, kernel->max(), kernel->min());
    bvpl_subgrid_iterator<float> output_iter(out_grid, kernel->max(), kernel->min());

    //kernel->print();
    while (!ocp_iter.isDone() && !app_iter.isDone()) {
      bvpl_kernel_iterator kernel_iter = kernel->iterator();
      bvpl_voxel_subgrid<float> ocp_subgrid = *ocp_iter;
      bvpl_voxel_subgrid<bsta_num_obs<bsta_gauss_f1> > app_subgrid = *app_iter;

      //reset the kernel iterator
      kernel_iter.begin();
      while (!kernel_iter.isDone()) {
        vgl_point_3d<int> idx = kernel_iter.index();
        float ocp_val;
        bsta_num_obs<bsta_gauss_f1> gauss_val;
        if (ocp_subgrid.voxel(idx, ocp_val) && app_subgrid.voxel(idx, gauss_val)) {
          //vcl_cout<< val << "at " << idx <<vcl_endl;
          bvpl_kernel_dispatch d = *kernel_iter;
          ocp_func_.apply(ocp_val, d);
          app_func_.apply(gauss_val,d);
        }
        ++kernel_iter;
      }
      // set the result at the output grid
      (*output_iter).set_voxel(ocp_func_.result()* app_func_.result().mean());
      ++ocp_iter;
      ++app_iter;
      ++output_iter;
    }
  }

 private:
  OCP_FUNC ocp_func_;
  APP_FUNC app_func_;
};

#endif
