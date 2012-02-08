// This is brl/bseg/boxm2/pro/processes/boxm2_compute_sun_affine_camera_process.cxx
//:
// \file
// \brief  A process for computing an affine camera, given a scene, and sun position
//
// \author Vishal Jain
// \date May 24, 2011

#include <bprb/bprb_func_process.h>
#include <boxm2/boxm2_scene.h>
#include <bpgl/algo/bpgl_camera_from_box.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vcl_cmath.h>
namespace boxm2_compute_sun_affine_camera_process_globals
{
  const unsigned n_inputs_ = 4;
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
  input_types_[3] = "bool";  // set to TRUE if el,az are in astronomical coord. system, i.e. degrees above horizon, degrees east of north
  bool result = pro.set_input_types(input_types_);

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";// affine sun camera
  output_types_[1] = "unsigned";// number of pixels (x)
  output_types_[2] = "unsigned";// number of pixels (y)
   
  result &= pro.set_output_types(output_types_);

  // set default for input[3], so as to not break old code
  brdb_value_sptr default_astro = new brdb_value_t<bool>(false);
  pro.set_input(3,default_astro);

  return result;
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
  vcl_cout << "scene = " << scene.ptr() << vcl_endl;
  float elevation  = pro.get_input<float>(1);
  float azimuthal  = pro.get_input<float>(2);
  bool astro_coords = pro.get_input<bool>(3);

  vgl_vector_3d<double> sun_dir_downwards;
  if (astro_coords){
    vcl_cout << "Using Astronomical Coordinate System" << vcl_endl; 
    double az_rads = vnl_math::pi/180 * azimuthal;
    double el_rads = vnl_math::pi/180 * elevation;
    sun_dir_downwards = -vgl_vector_3d<double>(vcl_cos(el_rads)*vcl_sin(az_rads),
                                               vcl_cos(el_rads)*vcl_cos(az_rads),
                                               vcl_sin(el_rads));
  }
  else {
  sun_dir_downwards = vgl_vector_3d<double>(-vcl_sin(elevation)*vcl_cos(azimuthal),
                                            -vcl_sin(elevation)*vcl_sin(azimuthal),
                                            -vcl_cos(elevation));
  }
  vcl_cout << "Sun ray direction = " << sun_dir_downwards << vcl_endl;
  vcl_map<boxm2_block_id, boxm2_block_metadata> blk_info=scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter = blk_info.begin();

  if(iter==blk_info.end()) return false;

  vgl_box_3d<double> box=scene->bounding_box();
  boxm2_block_metadata mdata = iter->second;
  unsigned int dimx = (unsigned int) vcl_floor(box.width()/(mdata.sub_block_dim_.x()/vcl_pow(2.0,(double)mdata.max_level_-1))+ 0.5);
  unsigned int dimy = (unsigned int) vcl_floor(box.height()/(mdata.sub_block_dim_.y()/vcl_pow(2.0,(double)mdata.max_level_-1))+ 0.5);

  vpgl_affine_camera<double> affine_camera  = bpgl_camera_from_box::affine_camera_from_box(box,
                                                       sun_dir_downwards,
                                                       dimx,
                                                       dimy);

  double u,v;
  affine_camera.project(box.min_x(),box.min_y(),box.min_z(),u,v);
  vcl_cout<<"Min "<<u<<","<<v<<" ";
  affine_camera.project(box.max_x(),box.max_y(),box.max_z(),u,v);
  vcl_cout<<"Max "<<u<<","<<v<<vcl_endl;

  vpgl_generic_camera<double> * gen_cam =new vpgl_generic_camera<double>();
  vpgl_generic_camera_convert::convert(affine_camera,dimx,dimy,*gen_cam);
  gen_cam->project(box.min_x(),box.min_y(),box.min_z(),u,v);
  vcl_cout<<"GMin "<<u<<","<<v<<" ";
  gen_cam->project(box.max_x(),box.max_y(),box.max_z(),u,v);
  vcl_cout<<"GMax "<<u<<","<<v<<vcl_endl;

  pro.set_output_val<vpgl_camera_double_sptr>(0, gen_cam);
  pro.set_output_val<unsigned>(1, dimx);
  pro.set_output_val<unsigned>(2, dimy);
  return true;
}
