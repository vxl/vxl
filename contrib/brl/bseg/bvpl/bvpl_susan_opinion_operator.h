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

#include "bvpl_subgrid_iterator.h"
#include "bvpl_kernel_iterator.h"
#include "bvpl_voxel_subgrid.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>

class bvpl_susan_opinion_operator
{
 public:
  bvpl_susan_opinion_operator() {}
  ~bvpl_susan_opinion_operator() {}

  // makes the neighborhood operation and stores the result in the output subgrid, which may be equal to the input
  void operate(bvxm_voxel_grid<vnl_float_3 >* dirgrid,
               bvxm_voxel_grid<bvxm_opinion>* opngrid,
               bvpl_kernel_sptr kernel, bvxm_voxel_grid<bvxm_opinion>* out_grid)
  {
    bvpl_subgrid_iterator<vnl_float_3 > sub_dir_iter(dirgrid, kernel->dim());
    bvpl_subgrid_iterator<bvxm_opinion> sub_opn_iter(opngrid, kernel->dim());
    bvpl_subgrid_iterator<bvxm_opinion> output_iter(out_grid, kernel->dim());
    while (!sub_dir_iter.isDone()) {
       bvpl_kernel_iterator kernel_iter = kernel->iterator();
       bvpl_voxel_subgrid<vnl_float_3> dirsubgrid = *sub_dir_iter;
       bvpl_voxel_subgrid<bvxm_opinion> opnsubgrid = *sub_opn_iter;

       vnl_float_3 cur_normal=(*sub_dir_iter).get_voxel();
       bvxm_opinion cur_opn=(*sub_opn_iter).get_voxel();

       vgl_plane_3d<double> p(cur_normal[0],cur_normal[1],cur_normal[2],0);
       vcl_vector<float> thetai;
       vcl_vector<float> wi;

       //reset the iterator
       kernel_iter.begin();
       float tot=0.0;
       float mu=0;
       while (!kernel_iter.isDone()) {
         vgl_point_3d<int> idx = kernel_iter.index();
         vnl_float_3 normal;
         bvxm_opinion opn;
         if (dirsubgrid.voxel(idx, normal) && opnsubgrid.voxel(idx,opn))
         {
             bvpl_kernel_dispatch d = *kernel_iter;
             float theta=vcl_acos(dot_product<float,3>(cur_normal,normal));
             thetai.push_back(theta);
             wi.push_back(opn.b());
             mu+=theta*opn.b();
             tot+=opn.b();
         }
         ++kernel_iter;
       }
       if(tot!=0)
           mu/=tot;
       tot=0.0;
       float cnt=0.0;
       for(unsigned i=0;i<thetai.size();i++)
       {
           tot+=wi[i]*wi[i]*(thetai[i]-mu)*(thetai[i]-mu);
           cnt+=wi[i]*wi[i];
       }    
       float sig=0.0;
       if(cnt>0.0)
           sig=tot/cnt;

       // set the result at the output grid
       (*output_iter).set_voxel(bvxm_opinion(cur_opn.b()*vcl_sqrt(sig)));
       //(*output_iter).set_voxel(bvxm_opinion(cur_opn.b()*mu));
       ++sub_dir_iter;
       ++sub_opn_iter;
       ++output_iter;


    }
    
  }

 private:
};

#endif
