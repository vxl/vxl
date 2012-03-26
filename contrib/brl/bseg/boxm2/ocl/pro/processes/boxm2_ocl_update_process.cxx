// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene.
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vpl/vpl.h> // vpl_unlink()

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_update.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_process_globals
{
  const unsigned n_inputs_  = 9;
  const unsigned n_outputs_ = 0;
  enum {
      UPDATE_SEGLEN = 0,
      UPDATE_PREINF = 1,
      UPDATE_PROC   = 2,
      UPDATE_BAYES  = 3,
      UPDATE_CELL   = 4
  };

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels,vcl_string opts)
  {
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
    vec_kernels.push_back(seg_len);

    bocl_kernel* pre_inf = new bocl_kernel();
    vcl_string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
    pre_inf->create_kernel(&device->context(),device->device_id(), src_paths, "pre_inf_main", pre_opts, "update::pre_inf");
    vec_kernels.push_back(pre_inf);

    //may need DIFF LIST OF SOURCES FOR THIS GUY
    bocl_kernel* proc_img = new bocl_kernel();
    vcl_string norm_opts = options + " -D PROC_NORM ";
    proc_img->create_kernel(&device->context(),device->device_id(), non_ray_src, "proc_norm_image", norm_opts, "update::proc_norm_image");
    vec_kernels.push_back(proc_img);

    //push back cast_ray_bit
    bocl_kernel* bayes_main = new bocl_kernel();
    vcl_string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
    bayes_main->create_kernel(&device->context(),device->device_id(), src_paths, "bayes_main", bayes_opt, "update::bayes_main");
    vec_kernels.push_back(bayes_main);

    //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    bocl_kernel* update = new bocl_kernel();
    vcl_string update_opt = options + " -D UPDATE_BIT_SCENE_MAIN ";
    update->create_kernel(&device->context(),device->device_id(), non_ray_src, "update_bit_scene_main", update_opt, "update::update_main");
    vec_kernels.push_back(update);

    return ;
  }

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_update_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image
  input_types_[5] = "vcl_string";                   //illumination identifier
  input_types_[6] = "vil_image_view_base_sptr";     //mask image view
  input_types_[7] = "bool";                         //do_update_alpha/don't update alpha
  input_types_[8] = "float";                        //variance value? if 0.0 or less, then use variable variance

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default 6 and 7 and 8 inputs
  brdb_value_sptr idx        = new brdb_value_t<vcl_string>("");
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  brdb_value_sptr up_alpha   = new brdb_value_t<bool>(true);  //by default update alpha
  brdb_value_sptr def_var    = new brdb_value_t<float>(-1.0f);
  pro.set_input(5, idx);
  pro.set_input(6, empty_mask);
  pro.set_input(7, up_alpha);
  pro.set_input(8, def_var);
  return good;
}

bool boxm2_ocl_update_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_process_globals;

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string               ident        = pro.get_input<vcl_string>(i++);
  vil_image_view_base_sptr mask_sptr    = pro.get_input<vil_image_view_base_sptr>(i++);
  bool                     update_alpha = pro.get_input<bool>(i++);
  float                    mog_var      = pro.get_input<float>(i++);

  //TODO Factor this out to a utility function
  //make sure this image small enough (or else carve it into image pieces)
  const vcl_size_t MAX_PIXELS = 16777216;
  if (img->ni()*img->nj() > MAX_PIXELS) {
    unsigned int sni = RoundUp(img->ni(), 16);
    unsigned int snj = RoundUp(img->nj(), 16);
    int numSegI = 1;
    int numSegJ = 1;
    while ( sni*snj > MAX_PIXELS/4 ) {
      sni /= 2;
      snj /= 2;
      numSegI++;
      numSegJ++;
    }
    sni = RoundUp(sni, 16);
    snj = RoundUp(snj, 16);
    vil_image_resource_sptr ir = vil_new_image_resource_of_view(*img);

    //run update for each image make sure to input i/j
    for (int i=0; i<numSegI+1; ++i) {
      for (int j=0; j<numSegJ+1; ++j) {
        //make sure the view doesn't extend past the original image
        vcl_size_t startI = (vcl_size_t) i * (vcl_size_t) sni;
        vcl_size_t startJ = (vcl_size_t) j * (vcl_size_t) snj;
        vcl_size_t endI = vcl_min(startI + sni, (vcl_size_t) img->ni());
        vcl_size_t endJ = vcl_min(startJ + snj, (vcl_size_t) img->nj());
        if (endI <= startI || endJ <= startJ)
          break;
        vcl_cout<<"Gettin patch: ("<<startI<<','<<startJ<<") -> ("<<endI<<','<<endJ<<')'<<vcl_endl;
        vil_image_view_base_sptr view = ir->get_copy_view(startI, endI-startI, startJ, endJ-startJ);
#if 0
        //test saving
        vcl_stringstream s;
        s<<"block_"<<startI<<'_'<<startJ<<".png";
        vil_save(*view, s.str().c_str());
#endif
        //run update
        boxm2_ocl_update::update(scene, device, opencl_cache, cam, view,
                                 ident, mask_sptr, update_alpha, mog_var,
                                 startI, startJ);
      }
    }
    return true;
  }

  //otherwise just run a normal update with one image
  boxm2_ocl_update::update(scene, device, opencl_cache, cam, img,
                           ident, mask_sptr, update_alpha, mog_var);
  return true;
}
