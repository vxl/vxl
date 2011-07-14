// This is brl/bseg/boxm2/pro/processes/boxm2_compute_sun_affine_camera_process.cxx
//:
// \file
// \brief  A process for computing a perspective camera, given a scene, camera center, and desired image resolution
//
// \author J. L. Mundy
// \date May 24, 2011

#include <bprb/bprb_func_process.h>
#include <boxm2/boxm2_scene.h>
#include <vpgl/algo/vpgl_camera_from_box.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vcl_cmath.h>
namespace boxm2_compute_sun_affine_camera_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 3;
}
bool boxm2_compute_sun_affine_camera_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_compute_sun_affine_camera_process_globals;

  //process takes 1 input, the scene
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "float"; //Elevation
  input_types_[2] = "float"; //Azimuthal angle of the sun

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";// longitude
  output_types_[1] = "unsigned";// longitude
  output_types_[2] = "unsigned";// longitude

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_compute_sun_affine_camera_process(bprb_func_process& pro)
{
  using namespace boxm2_compute_sun_affine_camera_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  float elevation  = pro.get_input<float>(1);
  float azimuthal  = pro.get_input<float>(2);


  vgl_vector_3d<double> sun_dir_downwards(-vcl_sin(elevation)*vcl_cos(azimuthal),
                                          -vcl_sin(elevation)*vcl_sin(azimuthal),
                                          -vcl_cos(elevation));

  vcl_map<boxm2_block_id, boxm2_block_metadata> blk_info=scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter = blk_info.begin();

  if(iter==blk_info.end()) return false;

  vgl_box_3d<double> box=scene->bounding_box();
  boxm2_block_metadata mdata = iter->second;
  unsigned int dimx = (unsigned int) vcl_floor(box.width()/(mdata.sub_block_dim_.x()/vcl_pow(2.0,(double)mdata.max_level_-1))+ 0.5);
  unsigned int dimy = (unsigned int) vcl_floor(box.height()/(mdata.sub_block_dim_.y()/vcl_pow(2.0,(double)mdata.max_level_-1))+ 0.5);

  vpgl_affine_camera<double> affine_camera  = vpgl_camera_from_box::affine_camera_from_box(box,
                                                       sun_dir_downwards,
                                                       dimx,
                                                       dimy);

  double u,v;
  affine_camera.project(box.min_x(),box.min_y(),box.min_z(),u,v);
  vcl_cout<<"Min "<<u<<","<<v<<" ";
  affine_camera.project(box.max_x(),box.max_y(),box.max_z(),u,v);
  vcl_cout<<"Max "<<u<<","<<v<<vcl_endl;

  vpgl_generic_camera<double> * gen_cam =new vpgl_generic_camera<double>();
  vpgl_generic_camera_compute::compute(affine_camera,dimx,dimy,*gen_cam);
  gen_cam->project(box.min_x(),box.min_y(),box.min_z(),u,v);
  vcl_cout<<"GMin "<<u<<","<<v<<" ";
  gen_cam->project(box.max_x(),box.max_y(),box.max_z(),u,v);
  vcl_cout<<"GMax "<<u<<","<<v<<vcl_endl;

  pro.set_output_val<vpgl_camera_double_sptr>(0, gen_cam);
  pro.set_output_val<unsigned>(1, dimx);
  pro.set_output_val<unsigned>(2, dimy);
  return true;
}
