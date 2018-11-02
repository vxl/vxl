// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_change_detection_process.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Ali Osman Ulusoy
// \date May 15, 2013

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <bprb/bprb_func_process.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/ocl/algo/bstm_ocl_change_detection.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_median.h>

// brdb stuff
#include <brdb/brdb_value.h>

// directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/util/boxm2_detect_change_blobs.h>

namespace bstm_ocl_change_detection_process_globals
{
  constexpr unsigned n_inputs_ = 9;
  constexpr unsigned n_outputs_ = 1;
}

bool bstm_ocl_change_detection_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_change_detection_process_globals;

  // process takes 9 inputs and two outputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "vcl_string"; // "raybelief" string for using raybelief
  input_types_[7] = "bool";       // true to use max mode probability
  input_types_[8] = "float";       // time

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // prob of change image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default is 1x1, with no ray belief
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  brdb_value_sptr pmax = new brdb_value_t<bool>(false);    // use max-mode probability instead of mixture?
  pro.set_input(5, empty_mask);
  pro.set_input(7, pmax);
  return good;
}

bool bstm_ocl_change_detection_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_change_detection_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bocl_device_sptr         device = pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr          scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_opencl_cache_sptr   opencl_cache = pro.get_input<bstm_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_img = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string               norm_type = pro.get_input<std::string>(i++);
  bool                     pmax = pro.get_input<bool>(i++);
  auto                    time = pro.get_input<float>(i++);

  // img dims
  unsigned ni=img->ni();
  unsigned nj=img->nj();

  // allocate two output images
  auto*    change_img = new vil_image_view<float>(ni, nj);

  // check to see which type of change detection to do, either two pass, or regular
  vul_timer t;
  if ( norm_type != "single" ) {
    bstm_ocl_aux_pass_change::change_detect( *change_img,
                                               device,
                                               scene,
                                               opencl_cache,
                                               cam,
                                               img,
                                               mask_img,
                                               time);
  }
  else {
    // store scene smaprt pointer
    bstm_ocl_change_detection::change_detect( *change_img,
                                               device,
                                               scene,
                                               opencl_cache,
                                               cam,
                                               img,
                                               mask_img,
                                               norm_type,
                                               time);
  }
  std::cout<<" change time: "<<t.all()<<" ms"<<std::endl;

//  float thresh = 0.3;
//  //detect change blobs
//  std::vector<boxm2_change_blob> blobs;
//  boxm2_util_detect_change_blobs( *change_img,thresh,blobs );
//
//  //create a blob image
//  vil_image_view<float>* blobImg = new vil_image_view<float>(change_img->ni(), change_img->nj());
//  blobImg->fill(0.0f);
//  std::vector<boxm2_change_blob>::iterator iter;
//  for (iter=blobs.begin(); iter!=blobs.end(); ++iter)
//  {
//    //paint each blob pixel white
//    for (unsigned int p=0; p<iter->blob_size(); ++p) {
//      PairType pair = iter->get_pixel(p);
//      (*blobImg)( pair.x(), pair.y() ) = 255.0f;
//    }
//  }



  // set outputs
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, change_img);
  return true;
}
