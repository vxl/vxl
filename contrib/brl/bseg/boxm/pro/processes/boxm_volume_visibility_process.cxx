// This is brl/bseg/boxm/pro/processes/boxm_volume_visibility_process.cxx
//:
// \file
// \brief A class for obtaining visibility of a volume
//
// \author Vishal Jain
// \date Apr 30, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_render_image.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_compute_volume_visibility.h>
#include <boxm/boxm_sample_multi_bin.h>
#include <vil/vil_convert.h>

namespace boxm_volume_visibility_process_globals
{
  const unsigned n_inputs_ = 10;
  const unsigned n_outputs_ = 1;
}

bool boxm_volume_visibility_process_cons(bprb_func_process& pro)
{
  using namespace boxm_volume_visibility_process_globals;
  //process takes 4 inputs
  //input[0]: scene binary file
  //input[1]: camera
  //input[2]: ni of the expected image
  //input[3]: nj of the expected image
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "float";
  input_types_[5] = "float";
  input_types_[6] = "float";
  input_types_[7] = "float";
  input_types_[8] = "float";
  input_types_[9] = "float";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: rendered image
  // output[0]: mask
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  //output_types_[0] = "vil_image_view_base_sptr";
  //output_types_[1] = "vil_image_view_base_sptr";
  output_types_[0] = "vil_image_view_base_sptr";

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool boxm_volume_visibility_process(bprb_func_process& pro)
{
  using namespace boxm_volume_visibility_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);


  float min_x=pro.get_input<float>(i++);
  float min_y=pro.get_input<float>(i++);
  float min_z=pro.get_input<float>(i++);

  float max_x=pro.get_input<float>(i++);
  float max_y=pro.get_input<float>(i++);
  float max_z=pro.get_input<float>(i++);

  vgl_box_3d<double> query(min_x,min_y,min_z,
                           max_x,max_y,max_z);

  double xverts[8];
  double yverts[8];

  boxm_utils::project_corners(boxm_utils::corners_of_box_3d(query),camera,xverts,yverts);
  boct_face_idx face_id=boxm_utils::visible_faces(query,camera,xverts,yverts);
  vil_image_view_base_sptr img;


  // check the scene's app model
  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
    float val=1.0;

    if (!scene_ptr->multi_bin()){
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      val=boxm_compute_volume_visibility<short, boxm_sample<BOXM_APM_MOG_GREY> >(query,*scene, camera);
    }
    else
    {
      typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      val=boxm_compute_volume_visibility<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(query,*scene, camera);
    }
    vil_image_view<float> img_vol(ni,nj);img_vol.fill(0.0);
    boxm_utils::project_cube_fill_val(face_id,img_vol,val,xverts,yverts);
    vil_image_view<unsigned char> *vol_vis = new vil_image_view<unsigned char>(img_vol.ni(),img_vol.nj(),img_vol.nplanes());
    vil_convert_stretch_range_limited(img_vol,*vol_vis, 0.0f, 1.0f);
    img = vol_vis;

    vcl_cout<<"\n Visibility is "<<val;
  }
  else {
    vcl_cout << "boxm_volume_visibility_process: undefined APM type" << vcl_endl;
    return false;
  }

  unsigned j = 0;
  pro.set_output_val<vil_image_view_base_sptr>(j++, img);
  return true;
}
