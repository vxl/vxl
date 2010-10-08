#include "icam_view_metadata.h"

#include <vgl/vgl_box_3d.h>

void icam_view_metadata::register_image(vil_image_view<double> const& source_img)
{
  // set the source to the minimizer
  minimizer_->set_source_img(source_img);

  vgl_box_3d<double> trans_box(-0.5,-0.5,-0.5,0.5,0.5,0.5); 
  vgl_vector_3d<double>  trans_steps(0.25,0.25,0.25);

  // take the coarsest level
  unsigned level = minimizer_->n_levels()-1;
  double min_allowed_overlap=0.5;

  // return params
  vgl_vector_3d<double> min_trans;
  vgl_rotation_3d<double> min_rot;
  double min_overlap_fraction;
  if (minimizer_->exhaustive_camera_search(trans_box,trans_steps,level,min_allowed_overlap,min_trans,min_rot,min_cost_,min_overlap_fraction)) {
      vcl_cout << "Trans" << min_trans << vcl_endl;
      vcl_cout << "Rot" << min_rot << vcl_endl;
      vcl_cout << "Cost" << min_cost_ << vcl_endl;
  }

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
