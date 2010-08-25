// This is gel/mrc/vpgl/ihog/ihog_minimizer.cxx
//:
// \file

#include "ihog_minimizer.h"

#include <ihog/ihog_cost_func.h>
#include <ihog/ihog_lsqr_cost_func.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vil/vil_math.h>
#include <vcl_cstdlib.h>


#include <vnl/vnl_matlab_filewrite.h>


//: Constructor
ihog_minimizer::ihog_minimizer( const ihog_image<float>& image1,
                                const ihog_image<float>& image2,
                                const ihog_world_roi& roi )
                                : end_error_(0.0), from_mask_(false), to_mask_(false)
{
  ihog_world_roi roi_L(roi);
  int levels = 0;

  while (roi_L.size_in_u() > min_level_size_ && roi_L.size_in_v() > min_level_size_){
    roi_pyramid_.push_back(roi_L);
    roi_L.set_size_in_u((roi_L.size_in_u()+1)/2);
    roi_L.set_size_in_v((roi_L.size_in_v()+1)/2);
    vgl_point_2d<double> p(roi_L.origin().x()/2.0, roi_L.origin().y()/2.0);
    roi_L.set_origin(p);
    ++levels;
  }
  vcl_cout << "LEVELS = "<<levels<<'\n';

  if (levels == 0) {
    levels = 1;
    roi_pyramid_.push_back(roi_L);
  }

  form1_=image1.world2im();
  form2_=image2.world2im();

  from_pyramid_.set_max_level(levels);
  to_pyramid_.set_max_level(levels);
  from_pyramid_=vil_pyramid_image_view<float>(image1.image());
  to_pyramid_=vil_pyramid_image_view<float>(image2.image());
}

//: Constructor with a mask
ihog_minimizer::ihog_minimizer( const ihog_image<float>& image1,
                                  const ihog_image<float>& image2,
                                  const ihog_image<float>& image_mask,
                                  const ihog_world_roi& roi, bool image1_mask )
                                  : end_error_(0.0), from_mask_(image1_mask), to_mask_(!image1_mask)
{
  ihog_world_roi roi_L(roi);
  int levels = 0;

  while (roi_L.size_in_u() > min_level_size_ && roi_L.size_in_v() > min_level_size_){
    roi_pyramid_.push_back(roi_L);
    roi_L.set_size_in_u((roi_L.size_in_u()+1)/2);
    roi_L.set_size_in_v((roi_L.size_in_v()+1)/2);
    vgl_point_2d<double> p(roi_L.origin().x()/2.0, roi_L.origin().y()/2.0);
    roi_L.set_origin(p);
    ++levels;
  }

  if (levels == 0){
    levels = 1;
    roi_pyramid_.push_back(roi_L);
  }

  form1_=image1.world2im();
  form2_=image2.world2im();

  from_pyramid_.set_max_level(levels);
  to_pyramid_.set_max_level(levels);
  from_pyramid_=vil_pyramid_image_view<float>(image1.image());
  to_pyramid_=vil_pyramid_image_view<float>(image2.image());
  if (from_mask_) {
    from_mask_pyramid_.set_max_level(levels); 
    from_mask_pyramid_=vil_pyramid_image_view<float>(image_mask.image());
    mask_form1_=image_mask.world2im();
  } else {
    to_mask_pyramid_.set_max_level(levels); 
    to_mask_pyramid_=vil_pyramid_image_view<float>(image_mask.image());
    mask_form2_=image_mask.world2im();
  }
}

//: Constructor with two masks
ihog_minimizer::ihog_minimizer( const ihog_image<float>& image1,
                                 const ihog_image<float>& image2,
                                 const ihog_image<float>& image1_mask,
                                 const ihog_image<float>& image2_mask,
                                 const ihog_world_roi& roi)
                                 : end_error_(0.0), from_mask_(true), to_mask_(true)
{
  ihog_world_roi roi_L(roi);
  int levels = 0;

  while (roi_L.size_in_u() > min_level_size_ && roi_L.size_in_v() > min_level_size_){
    roi_pyramid_.push_back(roi_L);
    roi_L.set_size_in_u((roi_L.size_in_u()+1)/2);
    roi_L.set_size_in_v((roi_L.size_in_v()+1)/2);
    vgl_point_2d<double> p(roi_L.origin().x()/2.0, roi_L.origin().y()/2.0);
    roi_L.set_origin(p);
    ++levels;
  }

  if (levels == 0){
    levels = 1;
    roi_pyramid_.push_back(roi_L);
  }

  form1_=image1.world2im();
  form2_=image2.world2im();
  mask_form1_=image1_mask.world2im();
  mask_form2_=image2_mask.world2im();
  from_pyramid_.set_max_level(levels);
  to_pyramid_.set_max_level(levels);
  from_mask_pyramid_.set_max_level(levels);
  to_mask_pyramid_.set_max_level(levels);
  from_pyramid_=vil_pyramid_image_view<float>(image1.image());
  to_pyramid_=vil_pyramid_image_view<float>(image2.image());
  from_mask_pyramid_=vil_pyramid_image_view<float>(image1_mask.image());
  to_mask_pyramid_=vil_pyramid_image_view<float>(image2_mask.image());
}


//: The main function.
void
ihog_minimizer::minimize(ihog_transform_2d& xform)
{
  //xform.set_rigid_body(1.0, 0.0, 0.0);

  ihog_transform_2d::Form form = xform.form();

  vnl_vector<double> param, fx;
  xform.params(param);
  double init_scale = vcl_pow(0.5,from_pyramid_.nlevels()+1);

  ihog_transform_2d undo_xform;
  undo_xform.set_zoom_only(1.0/init_scale,1.0/init_scale,0.0,0.0);
  ihog_transform_2d undo_step;
  undo_step.set_zoom_only(0.5,0.5,0.0,0.0);
  vcl_cout << "xform.origin = "<<xform.origin()<<'\n';
  xform.set_origin( vgl_point_2d<double>(xform.origin().x()*init_scale,
                                     xform.origin().y()*init_scale) );
  vcl_cout << "new xform.origin = "<<xform.origin()<<'\n';

  //vnl_matlab_filewrite matlab("C:/MATLAB/work/vxl.mat");
  //vnl_matrix<double> result(50,50);

  for (int L=from_pyramid_.nlevels(); L>=0; --L)
  {
    xform.set_origin( vgl_point_2d<double>(xform.origin().x()*2.0,
                                           xform.origin().y()*2.0) );

    undo_xform = undo_xform * undo_step;
    ihog_image<float> image1(from_pyramid_(L),form1_);
    ihog_image<float> image2(to_pyramid_(L),form2_);


    ihog_image<float> im1(image1);
    ihog_image<float> im2(image2);
    vil_gauss_filter_5tap(image1.image(),im1.image(),vil_gauss_filter_5tap_params(2));
    vil_gauss_filter_5tap(image2.image(),im2.image(),vil_gauss_filter_5tap_params(2));
    im1.set_world2im(undo_xform*image1.world2im());
    im2.set_world2im(undo_xform*image2.world2im());

    ihog_lsqr_cost_func *cost;
    // no masks
    if (!from_mask_ && !to_mask_)
    {
      cost = new ihog_lsqr_cost_func(im1, im2, roi_pyramid_[L], xform);
    }
    // one mask
    else if (from_mask_ || to_mask_) {
      if (from_mask_) {
        ihog_image<float> image_mask(from_mask_pyramid_(L),mask_form1_);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_lsqr_cost_func( im1, im2, immask, roi_pyramid_[L], xform, true);
      }
      else {
        ihog_image<float> image_mask(to_mask_pyramid_(L),mask_form2_);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_lsqr_cost_func( im1, im2, immask, roi_pyramid_[L], xform, false);
      }
    }
    // both masks
    else
    {
      ihog_image<float> from_image_mask(from_mask_pyramid_(L),mask_form1_);
      ihog_image<float> f_immask(from_image_mask);
      f_immask.set_world2im(undo_xform*from_image_mask.world2im());

      ihog_image<float> to_image_mask(to_mask_pyramid_(L),mask_form2_);
      ihog_image<float> t_immask(to_image_mask);
      t_immask.set_world2im(undo_xform*to_image_mask.world2im());

      cost = new ihog_lsqr_cost_func( im1, im2, f_immask, t_immask, roi_pyramid_[L], xform);
    }
    vnl_levenberg_marquardt minimizer(*cost);
    //minimizer.set_x_tolerance(1e-16);
    //minimizer.set_f_tolerance(1.0);
    //minimizer.set_g_tolerance(1e-3);
    minimizer.set_trace(true);
    //minimizer.set_max_iterations(50);
    xform.params(param);
    minimizer.minimize(param);
    end_error_ = minimizer.get_end_error();
    xform.set(param,form);

    // scale the transformations for the next level;
    ihog_transform_2d scaling;
    scaling.set_zoom_only(0.5,0,0);
    form1_=scaling * image1.world2im();
    form2_=scaling * image2.world2im();
    if (from_mask_)
      mask_form1_=scaling * image1.world2im();
    if (to_mask_)
      mask_form2_=scaling * image2.world2im();
    delete cost;
  }
}

