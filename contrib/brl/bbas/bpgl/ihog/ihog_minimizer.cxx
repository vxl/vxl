// This is bbas/bpgl/ihog/ihog_minimizer.cxx
#include <iostream>
#include <cstdlib>
#include "ihog_minimizer.h"
//:
// \file

#include <ihog/ihog_lsqr_cost_func.h>
#include <ihog/ihog_minfo_cost_func.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_powell.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// generate a pyramid of transforms corresponding to the vil_image_pyramid
static  std::vector<ihog_transform_2d>
w2img_pyramid(ihog_transform_2d const& w2img, int n_levels)
{
  ihog_transform_2d temp = w2img;
  std::vector<ihog_transform_2d> ret;
  ret.push_back(w2img);
  ihog_transform_2d scaling;
  scaling.set_zoom_only(0.5,0,0);
  for (int i = 1; i<n_levels; ++i)
  {
    temp = scaling*temp;
    ret.push_back(temp);
  }
  return ret;
}
//: Constructor
ihog_minimizer::ihog_minimizer( const ihog_image<float>& image1,
                                const ihog_image<float>& image2,
                                const ihog_world_roi& roi )
  : end_error_(0.0), from_mask_(false), to_mask_(false)
{
  ihog_world_roi roi_L(roi);
  int levels = 0;

  while (roi_L.size_in_u() > min_level_size_ && roi_L.size_in_v() > min_level_size_) {
    roi_pyramid_.push_back(roi_L);
    roi_L.set_size_in_u((roi_L.size_in_u()+1)/2);
    roi_L.set_size_in_v((roi_L.size_in_v()+1)/2);
    vgl_point_2d<double> p(roi_L.origin().x()/2.0, roi_L.origin().y()/2.0);
    roi_L.set_origin(p);
    ++levels;
  }

  if (levels == 0) {
    levels = 1;
    roi_pyramid_.push_back(roi_L);
  }


  w2img1_ = w2img_pyramid(image1.world2im(), levels);
  w2img2_ = w2img_pyramid(image2.world2im(), levels);
    vil_image_view_base_sptr i1sptr =
      new vil_image_view<float>(image1.image());
  from_pyramid_=vil_pyramid_image_view<float>(i1sptr,levels);
  vil_image_view_base_sptr i2sptr = new vil_image_view<float>(image2.image());
  to_pyramid_=vil_pyramid_image_view<float>(i2sptr,levels);
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

  while (roi_L.size_in_u() > min_level_size_ && roi_L.size_in_v() > min_level_size_) {
    roi_pyramid_.push_back(roi_L);
    roi_L.set_size_in_u((roi_L.size_in_u()+1)/2);
    roi_L.set_size_in_v((roi_L.size_in_v()+1)/2);
    vgl_point_2d<double> p(roi_L.origin().x()/2.0, roi_L.origin().y()/2.0);
    roi_L.set_origin(p);
    ++levels;
  }

  if (levels == 0) {
    levels = 1;
    roi_pyramid_.push_back(roi_L);
  }

  w2img1_ = w2img_pyramid(image1.world2im(), levels);
  w2img2_ = w2img_pyramid(image2.world2im(), levels);

    vil_image_view_base_sptr i1sptr =
      new vil_image_view<float>(image1.image());
  from_pyramid_=vil_pyramid_image_view<float>(i1sptr,levels);
    vil_image_view_base_sptr i2sptr =
      new vil_image_view<float>(image2.image());
  to_pyramid_=vil_pyramid_image_view<float>(i2sptr,levels);

  if (from_mask_) {
    vil_image_view_base_sptr m1sptr =
      new vil_image_view<float>(image_mask.image());
    from_mask_pyramid_=vil_pyramid_image_view<float>(m1sptr,levels);
    w2mask_img1_ = w2img_pyramid(image_mask.world2im(), levels);
  }
  else {
    vil_image_view_base_sptr m2sptr =
      new vil_image_view<float>(image_mask.image());
    from_mask_pyramid_=vil_pyramid_image_view<float>(m2sptr,levels);
    to_mask_pyramid_=vil_pyramid_image_view<float>(m2sptr,levels);
    w2mask_img2_ = w2img_pyramid(image_mask.world2im(), levels);
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

  while (roi_L.size_in_u() > min_level_size_ && roi_L.size_in_v() > min_level_size_) {
    roi_pyramid_.push_back(roi_L);
    roi_L.set_size_in_u((roi_L.size_in_u()+1)/2);
    roi_L.set_size_in_v((roi_L.size_in_v()+1)/2);
    vgl_point_2d<double> p(roi_L.origin().x()/2.0, roi_L.origin().y()/2.0);
    roi_L.set_origin(p);
    ++levels;
  }

  if (levels == 0) {
    levels = 1;
    roi_pyramid_.push_back(roi_L);
  }
  w2img1_ = w2img_pyramid(image1.world2im(), levels);
  w2img2_ = w2img_pyramid(image2.world2im(), levels);
  w2mask_img1_ = w2img_pyramid(image1_mask.world2im(), levels);
  w2mask_img2_ = w2img_pyramid(image2_mask.world2im(), levels);
    vil_image_view_base_sptr i1sptr =
      new vil_image_view<float>(image1.image());
  from_pyramid_=vil_pyramid_image_view<float>(i1sptr,levels);
    vil_image_view_base_sptr i2sptr =
      new vil_image_view<float>(image2.image());
  to_pyramid_=vil_pyramid_image_view<float>(i2sptr,levels);

    vil_image_view_base_sptr m1sptr =
      new vil_image_view<float>(image1_mask.image());
    from_mask_pyramid_=vil_pyramid_image_view<float>(m1sptr,levels);
    vil_image_view_base_sptr m2sptr =
      new vil_image_view<float>(image2_mask.image());
    to_mask_pyramid_=vil_pyramid_image_view<float>(m2sptr,levels);
}

void ihog_minimizer::set_image1_mask(ihog_image<float>& mask)
{
  int levels = from_pyramid_.nlevels();
  vil_image_view_base_sptr msptr =
    new vil_image_view<float>(mask.image());

  from_mask_pyramid_=vil_pyramid_image_view<float>(msptr,levels);
  w2mask_img1_ = w2img_pyramid(mask.world2im(), levels);
  from_mask_=true;
}

void ihog_minimizer::set_image2_mask(ihog_image<float>& mask)
{
  int levels = to_pyramid_.nlevels();
  vil_image_view_base_sptr msptr =
    new vil_image_view<float>(mask.image());
  to_mask_pyramid_=vil_pyramid_image_view<float>(msptr,levels);
  w2mask_img2_ = w2img_pyramid(mask.world2im(), levels);
  to_mask_=true;
}
//: The main function.
void
ihog_minimizer::minimize(ihog_transform_2d& xform)
{
  ihog_transform_2d::Form form = xform.form();
  int n_levels = from_pyramid_.nlevels();
  vnl_vector<double> param, fx;
  xform.params(param);
  // the expression 1.0/(1<<X) is a bit more efficient than std::pow(0.5,X),
  // :       -- PVr
  double init_scale = 1.0/(1<<n_levels);

  ihog_transform_2d undo_xform;
  undo_xform.set_zoom_only(1.0/init_scale,0.0,0.0);
  ihog_transform_2d undo_step;
  undo_step.set_zoom_only(0.5,0.0,0.0);
  xform.set_origin( vgl_point_2d<double>(xform.origin().x()*init_scale,
                                         xform.origin().y()*init_scale) );

  for (int L=from_pyramid_.nlevels()-1; L>=0; --L)
  {
    xform.set_origin( vgl_point_2d<double>(xform.origin().x()*2.0,
                                           xform.origin().y()*2.0) );

    undo_xform = undo_xform * undo_step;
    ihog_image<float> image1(from_pyramid_(L),w2img1_[L]);
    ihog_image<float> image2(to_pyramid_(L),w2img2_[L]);

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
        ihog_image<float> image_mask(from_mask_pyramid_(L),w2mask_img1_[L]);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_lsqr_cost_func( im1, im2, immask, roi_pyramid_[L], xform, true);
      }
      else {
        ihog_image<float> image_mask(to_mask_pyramid_(L),w2mask_img2_[L]);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_lsqr_cost_func( im1, im2, immask, roi_pyramid_[L], xform, false);
      }
    }
    // both masks
    else
    {
      ihog_image<float> from_image_mask(from_mask_pyramid_(L),w2mask_img1_[L]);
      ihog_image<float> f_immask(from_image_mask);
      f_immask.set_world2im(undo_xform*from_image_mask.world2im());

      ihog_image<float> to_image_mask(to_mask_pyramid_(L),w2mask_img2_[L]);
      ihog_image<float> t_immask(to_image_mask);
      t_immask.set_world2im(undo_xform*to_image_mask.world2im());

      cost = new ihog_lsqr_cost_func( im1, im2, f_immask, t_immask, roi_pyramid_[L], xform);
    }
    vnl_levenberg_marquardt minimizer(*cost);
#if 0
    minimizer.set_x_tolerance(1e-16);
    minimizer.set_f_tolerance(1.0);
    minimizer.set_g_tolerance(1e-3);
    minimizer.set_max_iterations(50);
#endif
    minimizer.set_trace(true);
    xform.params(param);
    minimizer.minimize(param);
    end_error_ = minimizer.get_end_error();
    xform.set(param,form);
    delete cost;
  }
}

//: a sub-pixel minimization with an exhaustive initialization at each level of the pyramid
void ihog_minimizer::minimize_exhaustive_minfo(int radius, ihog_transform_2d& xform)
{
  ihog_transform_2d::Form form = xform.form();
  int n_levels = from_pyramid_.nlevels();
  vnl_vector<double> param, fx;
  xform.params(param);
  // the expression 1.0/(1<<X) is a bit more efficient than std::pow(0.5,X),
  // :       -- PVr
  double init_scale = 1.0/(1<<n_levels);

  ihog_transform_2d undo_xform;
  undo_xform.set_zoom_only(1.0/init_scale,0.0,0.0);
  ihog_transform_2d undo_step;
  undo_step.set_zoom_only(0.5,0.0,0.0);
  xform.set_origin( vgl_point_2d<double>(xform.origin().x()*init_scale,
                                         xform.origin().y()*init_scale) );
  double scaled_radius = radius*init_scale;

  for (int L=from_pyramid_.nlevels()-1; L>=0; --L)
  {
#ifdef DEBUG
    std::cout << "-- L: " << L << std::endl;
#endif
    xform.set_origin( vgl_point_2d<double>(xform.origin().x()*2.0,
                                           xform.origin().y()*2.0) );
    scaled_radius = scaled_radius*2.0;
#ifdef DEBUG
    std::cout << "--- scaled_radius: " << scaled_radius << std::endl;
#endif
    undo_xform = undo_xform * undo_step;
    ihog_image<float> image1(from_pyramid_(L),w2img1_[L]);
    ihog_image<float> image2(to_pyramid_(L),w2img2_[L]);

    ihog_image<float> im1(image1);
    ihog_image<float> im2(image2);
    vil_gauss_filter_5tap(image1.image(),im1.image(),vil_gauss_filter_5tap_params(2));
    vil_gauss_filter_5tap(image2.image(),im2.image(),vil_gauss_filter_5tap_params(2));
    im1.set_world2im(undo_xform*image1.world2im());
    im2.set_world2im(undo_xform*image2.world2im());

    ihog_minfo_cost_func *cost;
    // no masks
    if (!from_mask_ && !to_mask_)
    {
      cost = new ihog_minfo_cost_func(im1, im2, roi_pyramid_[L], xform);
    }
    // one mask
    else if (from_mask_ || to_mask_) {
      if (from_mask_) {
        ihog_image<float> image_mask(from_mask_pyramid_(L),w2mask_img1_[L]);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_minfo_cost_func( im1, im2, immask, roi_pyramid_[L], xform, true);
      }
      else {
        ihog_image<float> image_mask(to_mask_pyramid_(L),w2mask_img2_[L]);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_minfo_cost_func( im1, im2, immask, roi_pyramid_[L], xform, false);
      }
    }
    // both masks
    else
    {
      ihog_image<float> from_image_mask(from_mask_pyramid_(L),w2mask_img1_[L]);
      ihog_image<float> f_immask(from_image_mask);
      f_immask.set_world2im(undo_xform*from_image_mask.world2im());

      ihog_image<float> to_image_mask(to_mask_pyramid_(L),w2mask_img2_[L]);
      ihog_image<float> t_immask(to_image_mask);
      t_immask.set_world2im(undo_xform*to_image_mask.world2im());

      cost = new ihog_minfo_cost_func( im1, im2, f_immask, t_immask, roi_pyramid_[L], xform);
    }

    // now at this level first minimize using exhaustive search
    double min = 1000.0f; int min_tx, min_ty;
    int ix = int(xform.get_translation().x()),
        iy = int(xform.get_translation().y()),
        r  = int(scaled_radius/(from_pyramid_.nlevels()-L));
#ifdef DEBUG
    std::cout << "--- r: " << r << std::endl;
#endif
    for (int tx = ix-r; tx < ix + r+1; tx++) {
      for (int ty = iy-r; ty < iy + r+1; ty++) {
        //xform.set_translation_only(tx, ty);
        ihog_transform_2d tform;
        tform.set_translation_only(tx,ty);
        vnl_vector<double> param;
        tform.params(param);
        double mi = cost->f(param);
        if (mi < min) { min = mi; min_tx = tx; min_ty = ty; }
      }
    }
#ifdef DEBUG
    std::cout << "min_tx: " << min_tx << " min_ty: " << min_ty << std::endl;
#endif
    xform.set_translation_only(min_tx, min_ty);

    // now refine using Powell
    vnl_powell minimizer(cost);

#ifdef DEBUG
    minimizer.set_trace(true);
#endif
    xform.params(param);
    minimizer.minimize(param);
    end_error_ = minimizer.get_end_error();
    xform.set(param,form);
    delete cost;
  }
}

//: Run the minimization using mutual information cost
void ihog_minimizer::minimize_using_minfo(ihog_transform_2d& xform)
{
  ihog_transform_2d::Form form = xform.form();
  int n_levels = from_pyramid_.nlevels();
  vnl_vector<double> param, fx;
  xform.params(param);
  // the expression 1.0/(1<<X) is a bit more efficient than std::pow(0.5,X),
  // and it avoids having to #include <cmath> :       -- PVr
  double init_scale = 1.0/(1<<n_levels);

  ihog_transform_2d undo_xform;
  undo_xform.set_zoom_only(1.0/init_scale,0.0,0.0);
  ihog_transform_2d undo_step;
  undo_step.set_zoom_only(0.5,0.0,0.0);
  xform.set_origin( vgl_point_2d<double>(xform.origin().x()*init_scale,
                                         xform.origin().y()*init_scale) );

  std::cout << "initial:\n\txform,"
           << " ox: " << xform.origin().x()
           << " oy: " << xform.origin().y()
           << " tx: " << xform.get_translation().x()
           << " ty: " << xform.get_translation().y() << '\n';

  for (int L=from_pyramid_.nlevels()-1; L>=0; --L)
  {
    std::cout << "BEGIN level L: " << L << "\n\txform,"
             << " ox: " << xform.origin().x()
             << " oy: " << xform.origin().y()
             << " tx: " << xform.get_translation().x()
             << " ty: " << xform.get_translation().y() << '\n';

    xform.set_origin( vgl_point_2d<double>(xform.origin().x()*2.0,
                                           xform.origin().y()*2.0) );

    std::cout << "BEGIN level L after ADJUSTMENT: " << L << "\n\txform,"
             << " ox: " << xform.origin().x()
             << " oy: " << xform.origin().y()
             << " tx: " << xform.get_translation().x()
             << " ty: " << xform.get_translation().y() << '\n';

    undo_xform = undo_xform * undo_step;
    ihog_image<float> image1(from_pyramid_(L),w2img1_[L]);
    ihog_image<float> image2(to_pyramid_(L),w2img2_[L]);

    ihog_image<float> im1(image1);
    ihog_image<float> im2(image2);
    vil_gauss_filter_5tap(image1.image(),im1.image(),vil_gauss_filter_5tap_params(2));
    vil_gauss_filter_5tap(image2.image(),im2.image(),vil_gauss_filter_5tap_params(2));
    im1.set_world2im(undo_xform*image1.world2im());
    im2.set_world2im(undo_xform*image2.world2im());

    ihog_minfo_cost_func *cost;
    // no masks
    if (!from_mask_ && !to_mask_)
    {
      cost = new ihog_minfo_cost_func(im1, im2, roi_pyramid_[L], xform);
    }
    // one mask
    else if (from_mask_ || to_mask_) {
      if (from_mask_) {
        ihog_image<float> image_mask(from_mask_pyramid_(L),w2mask_img1_[L]);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_minfo_cost_func( im1, im2, immask, roi_pyramid_[L], xform, true);
      }
      else {
        ihog_image<float> image_mask(to_mask_pyramid_(L),w2mask_img2_[L]);
        ihog_image<float> immask(image_mask);
        immask.set_world2im(undo_xform*image_mask.world2im());
        cost = new ihog_minfo_cost_func( im1, im2, immask, roi_pyramid_[L], xform, false);
      }
    }
    // both masks
    else
    {
      ihog_image<float> from_image_mask(from_mask_pyramid_(L),w2mask_img1_[L]);
      ihog_image<float> f_immask(from_image_mask);
      f_immask.set_world2im(undo_xform*from_image_mask.world2im());

      ihog_image<float> to_image_mask(to_mask_pyramid_(L),w2mask_img2_[L]);
      ihog_image<float> t_immask(to_image_mask);
      t_immask.set_world2im(undo_xform*to_image_mask.world2im());

      cost = new ihog_minfo_cost_func( im1, im2, f_immask, t_immask, roi_pyramid_[L], xform);
    }

    vnl_powell minimizer(cost); // was: vnl_levenberg_marquardt minimizer(*cost);

#ifdef DEBUG
    minimizer.set_trace(true);
#endif
    xform.params(param);
    minimizer.minimize(param);
    end_error_ = minimizer.get_end_error();
    xform.set(param,form);
    delete cost;
    std::cout << "END level L: " << L << "\n\txform,"
             << " ox: " << xform.origin().x()
             << " oy: " << xform.origin().y()
             << " tx: " << xform.get_translation().x()
             << " ty: " << xform.get_translation().y() << '\n';
  }
  std::cout << "FINAL:\n\txform,"
           << " ox: " << xform.origin().x()
           << " oy: " << xform.origin().y()
           << " tx: " << xform.get_translation().x()
           << " ty: " << xform.get_translation().y() << '\n';
}
