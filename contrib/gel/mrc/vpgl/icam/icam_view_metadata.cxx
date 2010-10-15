#include "icam_view_metadata.h"

#include <vgl/vgl_box_3d.h>

void icam_view_metadata::register_image(vil_image_view<float> const& source_img)
{
  // set the source to the minimizer
  this->source_img_ = source_img;
  minimizer_->set_source_img(source_img);
 
  // take the coarsest level
  unsigned level = minimizer_->n_levels()-1;

  // return params
  vgl_vector_3d<double> min_trans;
  vgl_rotation_3d<double> min_rot;

  error_ = minimizer_->error(min_rot, min_trans, level);
  vcl_cout << "ERROR " << error_ << vcl_endl;
}

void icam_view_metadata::compute_camera()
{
  vgl_box_3d<double> trans_box(-0.5,-0.5,-0.5,0.5,0.5,0.5); 
  vgl_vector_3d<double>  trans_steps(0.25,0.25,0.25);

  // take the coarsest level
  unsigned final_level = 0; //minimizer_->n_levels()-1;
  double min_allowed_overlap=0.5;
  bool refine=true;

  // return params
  vgl_vector_3d<double> min_trans;
  vgl_rotation_3d<double> min_rot;
  double min_overlap;

  minimizer_->camera_search(trans_box,trans_steps,final_level,min_allowed_overlap,refine,min_trans,min_rot,error_,min_overlap); 
  vcl_cout << "ERROR " << error_ << vcl_endl;
  vcl_cout << min_trans << vcl_endl;
  vcl_cout << min_rot << vcl_endl;
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
