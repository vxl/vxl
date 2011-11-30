// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_change_detection_process.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_change_detection.h>

namespace boxm2_ocl_change_detection_process_globals
{
  const unsigned n_inputs_     = 9;
  const unsigned n_outputs_    = 2;
  //vcl_size_t local_threads [2] = {8,8};
  //vcl_size_t global_threads[2] = {8,8};

  ////pass two probability threshold (if greater than this, will do nxn on pixel)
  //const float PROB_THRESH = .1f;

  //static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;

  //void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*>& vec_kernels, vcl_string opts)
  //{
    ////gather all render sources... seems like a lot for rendering...
    ////gather all render sources... seems like a lot for rendering...
    //vcl_vector<vcl_string> src_paths;
    //vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    //src_paths.push_back(source_dir + "scene_info.cl");
    //src_paths.push_back(source_dir + "cell_utils.cl");
    //src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    //src_paths.push_back(source_dir + "backproject.cl");
    //src_paths.push_back(source_dir + "statistics_library_functions.cl");
    //src_paths.push_back(source_dir + "expected_functor.cl");
    //src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    //src_paths.push_back(source_dir + "bit/change_detection.cl");
    //src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    ////set kernel options
    //opts += " -D CHANGE -D DETERMINISTIC ";
    //vcl_string options=opts;
    //opts += " -D STEP_CELL=step_cell_change_detection_uchar8_w_expected(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.change,aux_args.change_exp,aux_args.intensity,aux_args.intensity_exp) ";

    ////have kernel construct itself using the context and device
    //bocl_kernel * ray_trace_kernel = new bocl_kernel();
    //ray_trace_kernel->create_kernel( &device->context(),
                                     //device->device_id(),
                                     //src_paths,
                                     //"change_detection_bit_scene",   //kernel name
                                     //opts,              //options
                                     //"boxm2 1x1 ocl change detection"); //kernel identifier (for error checking)
    //vec_kernels.push_back(ray_trace_kernel);

    ////create nxn kernel
    //vcl_stringstream pthresh;
    //pthresh<<" -D PROB_THRESH="<<PROB_THRESH<<"  ";
    //opts += pthresh.str();
    //bocl_kernel * nxn_kernel = new bocl_kernel();
    //nxn_kernel->create_kernel( &device->context(),
                               //device->device_id(),
                               //src_paths,
                               //"nxn_change_detection",
                               //opts,
                               //"boxm2 nxn ocl change detection kernel");
    //vec_kernels.push_back(nxn_kernel);

    ////create normalize image kernel
    //vcl_vector<vcl_string> norm_src_paths;
    //norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    //norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    //bocl_kernel * normalize_render_kernel=new bocl_kernel();
    //normalize_render_kernel->create_kernel( &device->context(),
                                            //device->device_id(),
                                            //norm_src_paths,
                                            //"normalize_change_kernel",   //kernel name
                                            //options,              //options
                                            //"normalize change detection kernel"); //kernel identifier (for error checking)

    //vec_kernels.push_back(normalize_render_kernel);
  //}
}

bool boxm2_ocl_change_detection_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "int";   //n for nxn ray casting
  input_types_[7] = "vcl_string"; //"raybelief" string for using raybelief
  input_types_[8] = "bool";       //true to use max mode probability

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  //prob of change image
  output_types_[1] = "vil_image_view_base_sptr";  //Red Green change image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default is 1x1, with no ray belief
  brdb_value_sptr nxn  = new brdb_value_t<int>(1);
  brdb_value_sptr rayb = new brdb_value_t<vcl_string>(""); //use ray belief? 
  brdb_value_sptr pmax = new brdb_value_t<bool>(false);    //use max-mode probability instead of mixture?
  pro.set_input(6, nxn);
  pro.set_input(7, rayb);
  pro.set_input(8, pmax);
  return good;
}

bool boxm2_ocl_change_detection_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr          device        = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr          scene         = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr   opencl_cache  = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr   cam           = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr  img           = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr  exp_img       = pro.get_input<vil_image_view_base_sptr>(i++);
  int                       n             = pro.get_input<unsigned>(i++);                 //nxn
  vcl_string                norm_type     = pro.get_input<vcl_string>(i++);
  bool                      pmax          = pro.get_input<bool>(i++);

  //img dims
  unsigned ni=img->ni();
  unsigned nj=img->nj();

  // store scene smaprt pointer
  vil_image_view<float>*    change_img     = new vil_image_view<float>(ni, nj); 
  vil_image_view<vxl_byte>* rgb_change_img = new vil_image_view<vxl_byte>(ni,nj,4); 
  boxm2_ocl_change_detection::change_detect( *change_img, 
                                             *rgb_change_img, 
                                             device, 
                                             scene,
                                             opencl_cache,
                                             cam,
                                             img,
                                             exp_img,
                                             n,
                                             norm_type,
                                             pmax ); 
  
  // set outputs
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, change_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, rgb_change_img);
  return true;
}
