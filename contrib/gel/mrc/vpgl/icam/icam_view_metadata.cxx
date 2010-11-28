#include "icam_view_metadata.h"

#include <vgl/vgl_box_3d.h>
#include <vil/vil_save.h>

icam_view_metadata::icam_view_metadata(vil_image_view<float> const& exp_img,
                                       vil_image_view<double> const& depth_img,
                                       icam_depth_transform const& dt)
{ 
  
  unsigned nbins = 16;
  unsigned min_pyramid_image_size = 16;
  unsigned box_reduction_k = 2;
  double local_min_thresh = 0.005;
  double smooth_sigma = 1.0;
  double axis_search_cone_multiplier = 10.0;
  double polar_range_multiplier = 2.0;
  bool verbose = true;
  vcl_string base_path = "";

  minimizer_=new icam_minimizer(exp_img, dt, min_pyramid_image_size, 
                                box_reduction_k, axis_search_cone_multiplier, 
                                polar_range_multiplier, local_min_thresh, 
                                smooth_sigma, nbins,
                                base_path, verbose); 
  final_level_ = minimizer_->n_levels() - 3;
}

void icam_view_metadata::register_image(vil_image_view<float> const& source_img)
{
  // set the source to the minimizer
  minimizer_->set_source_img(source_img);

  // take the coarsest level
  unsigned level = minimizer_->n_levels()-1;

  // return params
  vgl_box_3d<double> trans_box;
  trans_box.add(vgl_point_3d<double>(-.5, -.5, -.5));
  trans_box.add(vgl_point_3d<double>(.5, .5, .5));
  vgl_vector_3d<double> trans_steps(0.5, 0.5, 0.5);
  double min_allowed_overlap = 0.5, min_overlap;
  minimizer_->exhaustive_camera_search(trans_box,trans_steps,level,
                                       min_allowed_overlap,min_trans_,
                                       min_rot_, cost_,min_overlap);

  vcl_cout << " min translation " << min_trans_ << '\n';
  vcl_cout << " min rotation " << min_rot_.as_rodrigues() << '\n';
  vcl_cout << " registration cost " << cost_ << '\n'<< '\n';
}

void icam_view_metadata::refine_camera()
{

#if 0
  vil_image_view<float> image=this->minimizer_->dest(0);
  vil_save(image,"F:/tests/mundy-downtown/view_sphere/test/myimg.tiff");
#endif
  unsigned start_level = minimizer_->n_levels()-1;
  vgl_vector_3d<double> trans_steps(0.5, 0.5, 0.5);
  double min_overlap = 0.5, act_overlap;
  minimizer_->pyramid_camera_search(min_trans_, min_rot_,
                                    trans_steps,
                                    start_level,
                                    final_level_,
                                    min_overlap,
                                    false,
                                    min_trans_,
                                    min_rot_,
                                    cost_,
                                    act_overlap);

  vcl_cout << " Pyramid search result \n";
  vcl_cout << " min translation " << min_trans_ << '\n';
  vcl_cout << " min rotation " << min_rot_.as_rodrigues() << '\n';
  vcl_cout << " registration cost " << cost_ << '\n'<< '\n';


#if 0
  vil_image_view<float> img = minimizer_->view(min_rot,min_trans,0);
  vcl_stringstream s;
  s << "view" << cost_ << ".tiff";
  vcl_cout << "PATH=" << s.str().c_str() << vcl_endl;
  vil_save(img, s.str().c_str());
#endif
}

void icam_view_metadata::mapped_image(vil_image_view<float> const& source_img,
                                      vgl_rotation_3d<double>& rot,
                                      vgl_vector_3d<double>& trans, 
                                      unsigned level,
                                      vil_image_view<float>& act_dest,
                                      vil_image_view<float>& mapped_dest)
{
  // set the source to the minimizer
  minimizer_->set_source_img(source_img);
  act_dest = minimizer_->dest(level);
  mapped_dest = minimizer_->view(rot, trans, level);
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
