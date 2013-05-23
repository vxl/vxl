// This is brl/bseg/bstm/ocl/algo/bstm_ocl_update.cxx
#include "bstm_ocl_update.h"
//:
// \file
// \brief  A process for updating a model
//
// \author Ali Osman Ulusoy
// \date May 10, 2013

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vil/vil_image_view.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: Map of kernels should persist between process executions
vcl_map<vcl_string,vcl_vector<bocl_kernel*> > bstm_ocl_update::kernels_;

//Main public method, updates color model
bool bstm_ocl_update::update(bstm_scene_sptr         scene,
                              bocl_device_sptr         device,
                              bstm_opencl_cache_sptr  opencl_cache,
                              vpgl_camera_double_sptr  cam,
                              vil_image_view_base_sptr img)
{

}


//Returns vector of color update kernels (and caches them per device
vcl_vector<bocl_kernel*>& bstm_ocl_update::get_kernels(bocl_device_sptr device, vcl_string opts, bool isRGB)
{

}


//makes sure appearance types correspond correctly
bool bstm_ocl_update::validate_appearances(bstm_scene_sptr scene,
                                            vcl_string& data_type,
                                            int& appTypeSize,
                                            vcl_string& num_obs_type,
                                            vcl_string& options,
                                            bool& isRGB)
{

}
