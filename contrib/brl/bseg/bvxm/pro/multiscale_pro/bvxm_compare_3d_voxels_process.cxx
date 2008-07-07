#include "bvxm_compare_3d_voxels_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <bvxm/bvxm_voxel_world.h>
#include <multiscale/bvxm_multi_scale_voxel_world.h>
#include <multiscale/bvxm_multiscale_util.h>

#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>
#include <bsta/bsta_histogram.h>

#include <vcl_fstream.h>
#include <vcl_cstdio.h>

bvxm_compare_3d_voxels_process::bvxm_compare_3d_voxels_process()
{
  //inputs
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0] = "bvxm_voxel_world_sptr";   // world
  input_types_[1] = "bvxm_multi_scale_voxel_world_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";
  ////output
  //output_data_.resize(1,brdb_value_sptr(0));
  //output_types_.resize(1);
  //output_types_[0]= "float";      // output an expected image of the object at the highest prob location overlayed 

}


bool bvxm_compare_3d_voxels_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<bvxm_voxel_world_sptr>* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr main_world = input0->value();

  brdb_value_t<bvxm_multi_scale_voxel_world_sptr>* input1 =
    static_cast<brdb_value_t<bvxm_multi_scale_voxel_world_sptr>* >(input_data_[1].ptr());
  bvxm_multi_scale_voxel_world_sptr multi_scale_world = input1->value();

   brdb_value_t<unsigned>* input2 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned input_scale = input2->value();

  brdb_value_t<vcl_string>* input3 =
      static_cast<brdb_value_t<vcl_string>* >(input_data_[3].ptr());
  vcl_string filename = input3->value();

  //: for the ground truth
  bgeo_lvcs_sptr lvcs=main_world->get_params()->lvcs();


  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // get ocuppancy probability grids
  bvxm_voxel_grid_base_sptr gt_ocp_grid_base = main_world->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *gt_ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(gt_ocp_grid_base.ptr());
  //bvxm_voxel_grid<ocp_datatype>::const_iterator l_ocp_slab_it = lidar_ocp_grid->begin();

  bvxm_voxel_grid_base_sptr multi_ocp_grid_base = multi_scale_world->get_grid<OCCUPANCY>(0,input_scale);
  bvxm_voxel_grid<ocp_datatype> *multi_ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(multi_ocp_grid_base.ptr());

  vgl_vector_3d<unsigned int> grid_size = multi_scale_world->get_params()->num_voxels(input_scale);
  bsta_histogram<double> hist(0.0, 1.0, 20, 0.0);
  bvxm_voxel_grid<ocp_datatype>::const_iterator m_ocp_slab_it = multi_ocp_grid->begin();

  int factor=vcl_pow((float)2,(float)input_scale);
  for (unsigned k_idx = 0; k_idx < (unsigned)grid_size.z(); ++k_idx, ++m_ocp_slab_it) {
      bvxm_voxel_slab<float> m_slab = *m_ocp_slab_it;


        vgl_vector_3d<unsigned int> gt_grid_size = main_world->get_params()->num_voxels();

              int gt_k_idx=k_idx*factor;
              bvxm_voxel_grid<ocp_datatype>::const_iterator gt_ocp_slab_it = gt_ocp_grid->slab_iterator(gt_k_idx);
              bvxm_voxel_slab<float> gt_slab = *gt_ocp_slab_it;



      for (unsigned i=0; i<grid_size.x(); i++) {
          for (unsigned j=0; j<grid_size.y(); j++) {
              double min_err=2;
                      
                      for(int gt_i=i*factor-1;gt_i<=i*factor+1;gt_i++)
                      {
                          for(int gt_j=j*factor-1;gt_j<=j*factor+1;gt_j++)
                          {
                              if(gt_i>=0 && gt_j>=0 && gt_i<gt_grid_size.x() && gt_j<gt_grid_size.y())
                              {
                                  if(m_slab(i,j)>multi_scale_world->get_params()->min_occupancy_prob()
                                      && gt_slab(gt_i,gt_j) >main_world->get_params()->min_occupancy_prob())
                                  {     
                                       double err=2*vcl_fabs(m_slab(i,j)-gt_slab(gt_i,gt_j));
                                       err/=(m_slab(i,j)+gt_slab(gt_i,gt_j));
                                       if(err<min_err)
                                           min_err=err;
                                  }
                                      
                              }
                          }
                      }
              if(min_err<=1)
                hist.upcount(min_err, 1);   
          }
      }
  }

  vcl_ofstream ofile(filename.c_str());
  if(!ofile)
  {
      vcl_cerr<<"Couldn't open file "<<filename<< "\n";
      return false;
  }


  hist.print(ofile);

  ofile.close();

  return true;
}

