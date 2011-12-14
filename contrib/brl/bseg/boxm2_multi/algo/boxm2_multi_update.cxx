#include "boxm2_multi_update.h"
#include "boxm2_multi_store_aux.h"
#include "boxm2_multi_pre_vis_inf.h"
#include "boxm2_multi_update_cell.h"

#include <vcl_algorithm.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <bocl/bocl_manager.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>

vcl_map<vcl_string,vcl_vector<bocl_kernel*> > boxm2_multi_update::kernels_; 

float boxm2_multi_update::update(       boxm2_multi_cache&              cache,
                                  const vil_image_view<float>&          img,
                                        vpgl_camera_double_sptr         cam)
{
  vcl_cout<<"------------ boxm2_multi_update -----------------------"<<vcl_endl;
  
  //store aux data (cell vis, cell length) 
  boxm2_multi_store_aux::store_aux(cache, img, cam); 
  
  //calcl pre/vis inf, and store pre/vis images along the way
  float* norm_img = new float[img.ni() * img.nj()]; 
  vcl_map<bocl_device*, float*> pre_map, vis_map;
  boxm2_multi_pre_vis_inf::pre_vis_inf(cache, img, cam, vis_map, pre_map, norm_img); 
  
  //calculate cell beta, cell vis, and finally reduce each cell to new alphas
  boxm2_multi_update_cell::update_cells(cache, img, cam, vis_map, pre_map, norm_img); 
}


//-----------------------------------------------------------------
// returns vector of bocl_kernels for this specific device
//-----------------------------------------------------------------
vcl_vector<bocl_kernel*>& boxm2_multi_update::get_kernels(bocl_device_sptr device, vcl_string opts)
{
  // check to see if this device has compiled kernels already
  vcl_string identifier = device->device_identifier()+opts;
  if (kernels_.find(identifier) != kernels_.end()) 
    return kernels_[identifier]; 

  //if not, compile and cache them
  vcl_cout<<"===========Compiling multi update kernels===========\n"
          <<"  for device: "<<device->device_identifier()<<vcl_endl;
  vcl_vector<bocl_kernel*> kerns;

  //gather all render sources... seems like a lot for rendering...
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/update_kernels.cl");
  
  vcl_vector<vcl_string> non_ray_src = vcl_vector<vcl_string>(src_paths);
  src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  vcl_string options = opts+" -D INTENSITY  ";
  options += " -D DETERMINISTIC ";

  //create all passes
  bocl_kernel* seg_len = new bocl_kernel();
  vcl_string seg_opts = options + " -D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
  seg_len->create_kernel(&device->context(),device->device_id(), src_paths, "seg_len_main", seg_opts, "update::seg_len");
  kerns.push_back(seg_len);

  bocl_kernel* pre_inf = new bocl_kernel();
  vcl_string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
  pre_inf->create_kernel(&device->context(),device->device_id(), src_paths, "pre_inf_main", pre_opts, "update::pre_inf");
  kerns.push_back(pre_inf);

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  bocl_kernel* proc_img = new bocl_kernel();
  proc_img->create_kernel(&device->context(),device->device_id(), non_ray_src, "proc_norm_image", options, "update::proc_norm_image");
  kerns.push_back(proc_img);

  //push back cast_ray_bit
  bocl_kernel* bayes_main = new bocl_kernel();
  vcl_string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
  bayes_main->create_kernel(&device->context(),device->device_id(), src_paths, "bayes_main", bayes_opt, "update::bayes_main");
  kerns.push_back(bayes_main);

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  bocl_kernel* update = new bocl_kernel();
  update->create_kernel(&device->context(),device->device_id(), non_ray_src, "update_bit_scene_main", options, "update::update_main");
  kerns.push_back(update);
  
  //cache in map
  kernels_[identifier] = kerns; 
  return kernels_[identifier]; 
}


