// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_color_process.cxx
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <bprb/bprb_func_process.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_color.h>
#include <bocl/bocl_device.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vcl_algorithm.h>

namespace boxm2_ocl_update_color_process_globals
{
  const unsigned n_inputs_  = 8;
  const unsigned n_outputs_ = 0;
}

bool boxm2_ocl_update_color_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";
  input_types_[6] = "vcl_string";
  input_types_[7] = "bool";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //set defaults inputs
  brdb_value_sptr idx         = new brdb_value_t<vcl_string>("");
  brdb_value_sptr updateAlpha = new brdb_value_t<bool>(true);
  pro.set_input(5, idx);
  pro.set_input(6, idx);
  pro.set_input(7, updateAlpha);
  return good;
}

bool boxm2_ocl_update_color_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs: device, scene, opencl_cache, cam, img
  unsigned argIdx = 0;
  bocl_device_sptr          device = pro.get_input<bocl_device_sptr>(argIdx++);
  boxm2_scene_sptr          scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(argIdx++);
  vpgl_camera_double_sptr   cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  vil_image_view_base_sptr  img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vcl_string in_identifier = pro.get_input<vcl_string>(argIdx++);
  vcl_string mask_filename = pro.get_input<vcl_string>(argIdx++);
  bool       updateAlpha   = pro.get_input<bool>(argIdx++);

  //make sure this image small enough (or else carve it into image pieces)
  const vcl_size_t MAX_PIXELS = 16777216;
  if(img->ni()*img->nj() > MAX_PIXELS) {
    int sni = RoundUp(img->ni(), 16);  
    int snj = RoundUp(img->nj(), 16);
    int numSegI = 1;
    int numSegJ = 1;
    while( sni*snj > MAX_PIXELS/4 ) {
      sni /= 2; 
      snj /= 2;
      numSegI++;
      numSegJ++;
    }
    sni = RoundUp(sni, 16);
    snj = RoundUp(snj, 16);
    vil_image_resource_sptr ir = vil_new_image_resource_of_view(*img); 

    //run update for each image make sure to input i/j
    for(int i=0; i<numSegI+1; ++i) {
      for(int j=0; j<numSegJ+1; ++j) {
        //make sure the view doesn't extend past the original image
        vcl_size_t startI = (vcl_size_t) i * (vcl_size_t) sni; 
        vcl_size_t startJ = (vcl_size_t) j * (vcl_size_t) snj;
        vcl_size_t endI = vcl_min(startI + sni, (vcl_size_t) img->ni());
        vcl_size_t endJ = vcl_min(startJ + snj, (vcl_size_t) img->nj());
        if(endI <= startI || endJ <= startJ)
          break;
        vcl_cout<<"Gettin patch: ("<<startI<<','<<startJ<<") -> ("<<endI<<','<<endJ<<")"<<vcl_endl;
        vil_image_view_base_sptr view = ir->get_copy_view(startI, endI-startI, startJ, endJ-startJ);
        
        //run update
        boxm2_ocl_update_color::update_color(scene, device, opencl_cache, 
                                             cam, view, in_identifier, 
                                             mask_filename, updateAlpha,
                                             startI, startJ);
      }
    }
    return true;
  }

  //call ocl update
  boxm2_ocl_update_color::update_color(scene,device,opencl_cache,
                                       cam,img,in_identifier,
                                       mask_filename,updateAlpha);

  //no outputs
  return true;
}
