#include "icam_view_metadata.h"

#include <vgl/vgl_box_3d.h>
#include <vil/vil_save.h>

icam_view_metadata::icam_view_metadata(vil_image_view<float> const& exp_img,
                                       vil_image_view<double> const& depth_img,
                                       icam_depth_transform const& dt)
{ 
  

  unsigned min_pyramid_image_size = 16;
  unsigned box_reduction_k = 2;
  double local_min_thresh = 0.005;
  double axis_search_cone_multiplier = 10.0;
  double polar_range_multiplier = 2.0;
  bool verbose = true;
  vcl_string base_path = "";

  minimizer_=new icam_minimizer(exp_img, dt, min_pyramid_image_size, 
                                box_reduction_k, axis_search_cone_multiplier, 
                                polar_range_multiplier, local_min_thresh, 
                                base_path, verbose); 
}

void icam_view_metadata::register_image(vil_image_view<float> const& source_img)
{
  // set the source to the minimizer
  minimizer_->set_source_img(source_img);

  // take the coarsest level
  unsigned level = minimizer_->n_levels()-1;

  // return params
  vgl_vector_3d<double> min_trans;
  vgl_rotation_3d<double> min_rot;

  error_ = minimizer_->error(min_rot, min_trans, level);
  error_ *= -1.;
  vcl_cout << "ERROR " << error_ << vcl_endl;

}

void icam_view_metadata::compute_camera()
{
  vil_image_view<float> image=this->minimizer_->dest(0);
  vil_save(image,"F:/tests/mundy-downtown/view_sphere/test/myimg.tiff");

  vgl_box_3d<double> trans_box(-.5,-.5,-.5,.5,.5,.5); 
  vgl_vector_3d<double>  trans_steps(0.5,0.5,0.5);

  // take the coarsest level
  unsigned final_level = minimizer_->n_levels();
  double min_allowed_overlap=0.5;
  bool refine=false;

  // return params
  vgl_vector_3d<double> min_trans;
  vgl_rotation_3d<double> min_rot;
  double min_overlap;

  minimizer_->print();

  minimizer_->camera_search(trans_box,trans_steps,final_level-3,min_allowed_overlap,refine,min_trans,min_rot,error_,min_overlap); 
  vcl_cout << "ERROR " << error_ << vcl_endl;
  vcl_cout << min_trans << vcl_endl;
  vcl_cout << min_rot << vcl_endl;
  vil_image_view<float> img = minimizer_->view(min_rot,min_trans,0);
  vcl_stringstream s;
  s << "view" << error_ << ".tiff";
  vcl_cout << "PATH=" << s.str().c_str() << vcl_endl;
  vil_save(img, s.str().c_str());
}

void icam_view_metadata::b_read(vsl_b_istream& is)
{
  short version;
  vsl_b_read(is, version);
}

void icam_view_metadata::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
}

vcl_ostream& operator<<(vcl_ostream& os, icam_view_metadata const& p)
{
  p.print(os);
  return os;
}

void vsl_b_read(vsl_b_istream& is, icam_view_metadata& p)
{
  p.b_read(is);
}

void vsl_b_write(vsl_b_ostream& os, icam_view_metadata const& p)
{
  p.b_write(os);
}
