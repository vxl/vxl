#include <utility>
#include "icam_view_metadata.h"

#include <vgl/vgl_box_3d.h>
#include <vil/vil_load.h>



icam_view_metadata::icam_view_metadata(std::string  exp_img,
                                       std::string  dt)
  : exp_img_path_(std::move(exp_img)),depth_img_path_(std::move(dt))
{
}

void icam_view_metadata::create_minimizer(vil_image_view<float>*& exp_img, vil_image_view<double>*& depth_img,
                                          const vpgl_camera_double_sptr& camera, icam_minimizer_params const& params,
                                          icam_minimizer*& minimizer)
{
  vil_image_view_base_sptr exp=vil_load(exp_img_path_.c_str());
  vil_image_view_base_sptr depth=vil_load(depth_img_path_.c_str());
  if (load_image<float>(exp, exp_img) && load_image(depth, depth_img))  {
    auto* cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer());
    if (cam) {
      vnl_matrix_fixed<double, 3, 3> K = cam->get_calibration().get_matrix();
      const vgl_rotation_3d<double>& rot=cam->get_rotation();
      vgl_vector_3d<double> t=cam->get_translation();
      icam_depth_transform dt(K, *depth_img, rot, t);
      minimizer=new icam_minimizer (*exp_img, dt, params, false);
    }
  }
}

void icam_view_metadata::register_image(vil_image_view<float> const& source_img,
                                        vpgl_camera_double_sptr camera,
                                        icam_minimizer_params const& params)
{
  // create the images
  vil_image_view<float> *exp_img=nullptr;
  vil_image_view<double> *depth_img=nullptr;
  icam_minimizer* minimizer;
  create_minimizer(exp_img,depth_img,camera,params,minimizer);
  if (minimizer) {
    final_level_ = minimizer->n_levels() - 4;

    // set the source to the minimizer
    minimizer->set_source_img(source_img);

    // take the coarsest level
    unsigned level = minimizer->n_levels()-1;

    // return params
    vgl_box_3d<double> trans_box;
    trans_box.add(vgl_point_3d<double>(-.5, -.5, -.5));
    trans_box.add(vgl_point_3d<double>(.5, .5, .5));
    vgl_vector_3d<double> trans_steps(0.5, 0.5, 0.5);
    double min_allowed_overlap = 0.5, min_overlap;
    minimizer->exhaustive_camera_search(trans_box,trans_steps,level,
                                        min_allowed_overlap,min_trans_,
                                        min_rot_, cost_,min_overlap);

    std::cout << " min translation " << min_trans_ << '\n'
             << " min rotation " << min_rot_.as_rodrigues() << '\n'
             << " registration cost " << cost_ << '\n'<< std::endl;
    delete minimizer;
  }
  if (exp_img) delete exp_img;
  if (depth_img) delete depth_img;
}

void icam_view_metadata::refine_camera(vil_image_view<float> const& source_img,
                                       vpgl_camera_double_sptr camera,
                                       icam_minimizer_params const& params)
{
  vil_image_view<float> *exp_img;
  vil_image_view<double> *depth_img;
  icam_minimizer* minimizer;
  create_minimizer(exp_img,depth_img,camera,params,minimizer);
  if (minimizer) {
    final_level_ = minimizer->n_levels() - 4;
    unsigned start_level = minimizer->n_levels()-1;
    vgl_vector_3d<double> trans_steps(0.5, 0.5, 0.5);
    double min_overlap = 0.5, act_overlap;
    // set the source to the minimizer
    minimizer->set_source_img(source_img);
    minimizer->pyramid_camera_search(min_trans_, min_rot_,
                                     trans_steps,
                                     start_level,
                                     final_level_,
                                     min_overlap,
                                     false,
                                     min_trans_,
                                     min_rot_,
                                     cost_,
                                     act_overlap);

    std::cout << " Pyramid search result\n"
             << " min translation " << min_trans_ << '\n'
             << " min rotation " << min_rot_.as_rodrigues() << '\n'
             << " registration cost " << cost_ << '\n'<< std::endl;
    delete minimizer;
  }
  if (exp_img) delete exp_img;
  if (depth_img) delete depth_img;

#if 0
  vil_image_view<float> img = minimizer_->view(min_rot,min_trans,0);
  std::stringstream s;
  s << "view" << cost_ << ".tiff";
  std::cout << "PATH=" << s.str().c_str() << std::endl;
  vil_save(img, s.str().c_str());
#endif
}

void icam_view_metadata::mapped_image(vil_image_view<float> const& source_img,
                                      const vpgl_camera_double_sptr& camera,
                                      vgl_rotation_3d<double>& rot,
                                      vgl_vector_3d<double>& trans,
                                      unsigned level,
                                      icam_minimizer_params const& params,
                                      vil_image_view<float>& act_dest,
                                      vil_image_view<float>& mapped_dest)
{
  // set the source to the minimizer
  vil_image_view<float> *exp_img;
  vil_image_view<double> *depth_img;
  icam_minimizer* minimizer;
  create_minimizer(exp_img,depth_img,camera,params,minimizer);
  if (minimizer) {
    minimizer->set_source_img(source_img);
    act_dest = minimizer->dest(level);
    mapped_dest = minimizer->view(rot, trans, level);
    delete minimizer;
  }
  if (exp_img) delete exp_img;
  if (depth_img) delete depth_img;
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

std::ostream& operator<<(std::ostream& os, icam_view_metadata const& p)
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
