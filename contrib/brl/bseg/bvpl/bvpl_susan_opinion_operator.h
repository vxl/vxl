#ifndef bvpl_susan_opinion_operator_h_
#define bvpl_susan_opinion_operator_h_
//:
// \file
// \brief A class for performing SUSAN operator on normal grid
//
// \author Vishal Jain
// \date Aug , 2009
// \verbatim
//  Modifications
//  Gamze Tunali June 5, 2009
//      operate() method taking the normal and response grids  and finds out voxels with
//
// \endverbatim

#include <iostream>
#include "bvpl_subgrid_iterator.h"
#include <bvpl/kernels/bvpl_kernel_iterator.h>
#include "bvpl_voxel_subgrid.h"
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvxm/grid/bvxm_opinion.h>

#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvpl_susan_opinion_operator
{
 public:
  bvpl_susan_opinion_operator() = default;
  ~bvpl_susan_opinion_operator() = default;

  // makes the neighborhood operation and stores the result in the output subgrid, which may be equal to the input
  void operate(bvxm_voxel_grid<int >* dirgrid,
               bvxm_voxel_grid<bvxm_opinion>* opngrid,
               bvpl_kernel_vector_sptr kernel,
               bvxm_voxel_grid<bvxm_opinion>* out_grid)
  {
    std::cout<<"Operator"<<std::endl
            <<"Min "<<kernel->min()<<" Max "<< kernel->max()<<std::endl;
    bvpl_subgrid_iterator<int > sub_dir_iter(dirgrid, kernel->min(), kernel->max());
    bvpl_subgrid_iterator<bvxm_opinion> sub_opn_iter(opngrid, kernel->min(), kernel->max());
    bvpl_subgrid_iterator<bvxm_opinion> output_iter(out_grid, kernel->min(), kernel->max());
    while (!sub_dir_iter.isDone())
    {
      bvpl_voxel_subgrid<int> dirsubgrid = *sub_dir_iter;
      bvpl_voxel_subgrid<bvxm_opinion> opnsubgrid = *sub_opn_iter;
      int id=(*sub_dir_iter).get_voxel();
      if (id>-1)
      {
        bvpl_kernel_sptr cur_kernel=kernel->kernels_[id];
        vnl_float_3 cur_normal=cur_kernel->axis();

        bvpl_kernel_iterator kernel_iter = cur_kernel->iterator();

        bvxm_opinion cur_opn=(*sub_opn_iter).get_voxel();
        std::vector<float> thetai;
        std::vector<float> wi;

        //reset the iterator
        kernel_iter.begin();
        float tot=0.0;
        float mu=0;

        while (!kernel_iter.isDone()) {
          vgl_point_3d<int> idx = kernel_iter.index();
          int kernel_id;
          bvxm_opinion opn;
          if (dirsubgrid.voxel(idx, kernel_id) && opnsubgrid.voxel(idx,opn))
          {
            bvpl_kernel_dispatch d = *kernel_iter;

            if (kernel_id>-1)
            {
              vnl_float_3 normal=kernel->kernels_[kernel_id]->axis();
              float dot_prod=dot_product<float,3>(cur_normal,normal);
              float measure_dot_prod=1-std::fabs(dot_prod);

              thetai.push_back(measure_dot_prod);
              wi.push_back(opn.b());
              mu+=measure_dot_prod*opn.b();
              tot+=opn.b();
            }
          }
          ++kernel_iter;
        }
        if (tot!=0) mu/=tot;
        // reset tot variable
        tot=0.0;
        float cnt=0.0;
        for (unsigned i=0;i<thetai.size();++i)
        {
          tot+=wi[i]*(thetai[i]-mu)*(thetai[i]-mu);
          cnt+=wi[i];
        }

        float sig=0.0;
        if (cnt>0.0)
          sig=tot/cnt;
        float weight_sig=0.0;
        if (sig!=0.0)
          weight_sig=1-std::exp(-mu*mu/(sig));
        // set the result at the output grid
        (*output_iter).set_voxel(bvxm_opinion(cur_opn.b()*weight_sig));
      }
      else
        (*output_iter).set_voxel(bvxm_opinion(0));
      ++sub_dir_iter;
      ++sub_opn_iter;
      ++output_iter;
    }
  }
};

#endif // bvpl_susan_opinion_operator_h_
